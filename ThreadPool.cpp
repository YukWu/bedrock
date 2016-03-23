#include <stddef.h>
#include <iostream>
#include <stdlib.h>

#include <utility>


#include "ThreadPool.hpp"


using namespace bedrock;
using std::function;
using std::mutex;
using std::thread;

pthread_key_t  bedrock::G_WORKER_KEY ;




/**
  Default constructor
  @param workers Number of threads this pool should use
  @param active  Indicates whether worker threads are
         immediately created; defaults to TRUE
**/
ThreadPool::ThreadPool(unsigned int workers) :_current_worker(0) {
	//size up workers vector
	_workers.reserve(workers);

    pthread_key_create(&G_WORKER_KEY, NULL);// 只执行一次

	//initialize workers
	for(unsigned int i=0;i<workers;i++){

        _workers.push_back(std::unique_ptr<Worker>( new Worker(this,i)));
	}
}

ThreadPool::~ThreadPool(){
}

/**
 	Enqueue a callback for processing by any of the workers.
	No guarantee is made as to which worker will process this
	callback

	@param cb   Callback object for worker thread to process
	@returns ID of worker thread used for processing
 **/
int ThreadPool::enqueue(std::function<void()> cb){
	int result = getNextThreadID();
	_workers[result]->enqueue(cb);
	return result;
}


/**
 	Enqueue a callback for processing with a particular worker.
	The key is used to select a worker; if the same key is used
	multiple times,requests will be processed in-order.

	@param cb  Callback object for worker thread to process
	@param id  ID of worker thread to process with
	@param delete_functor Should the thread pool free the callback after call it?
 **/

void ThreadPool::enqueue(std::function<void()> cb ,int id){
	_workers[ id % _workers.size()]->enqueue(cb);
}



/**
 	Get the next worker thread id for enqueing into.
	@return ID of a worker thread
 **/

int ThreadPool::getNextThreadID(void){
    std::lock_guard<mutex> mp(_current_worker_lock);

	int result = (_current_worker++) % _workers.size();

	return result;
}



/**
 	Retrieve the ThreadPool that the calling function is executing on.
	@returns ThreadPool pointer if the calling function is executing on
	a thread which exists within a pool; otherwise, returns NULL
 **/

ThreadPool* ThreadPool::getCurrentThreadPool(){
	ThreadPool::Worker* w = (ThreadPool::Worker*)pthread_getspecific(G_WORKER_KEY) ; //使用线程局部存储
	if(w!=nullptr)
		return w->_owner;
	else
		return nullptr;
}

/**
	Retrieve the thread key of a worker thread that the calling
	function is executing on.
	@return -1 if the calling function isn't executing on a thread
	within a ThreadPool; otherwise returns the thread key as an int value.
 **/

int ThreadPool::getCurrentThreadKey(){

	ThreadPool::Worker* w = (ThreadPool::Worker*)pthread_getspecific(G_WORKER_KEY);
	if(w!=nullptr)
		return w->_threadkey;
	else
		return -1;
}

void ThreadPool::waitForAllEnd(function<void()> cb){

    std::atomic_int c = { static_cast<int>(_workers.size())};

    for( auto i = _workers.cbegin(); i!=_workers.end(); i++ ){
        (*i)->enqueue([&c,cb,this](){ c--; if(c==0){this->enqueue(cb);}});
    }

}

void ThreadPool::waitForAllEndBlock(function<void()> cb){
    std::atomic_int c = { static_cast<int>(_workers.size())};
    for( auto i = _workers.cbegin(); i!= _workers.end();i++){
        (*i)->enqueue([&c](){ c--;});
    }
    while( c );
    cb();
}
//-----------------------
// ThreadPool::Worker
//-----------------------

ThreadPool::Worker::Worker(ThreadPool* owner,int threadkey):
_owner(owner),_threadkey(threadkey),_active(true){

    _thread = thread(ThreadPool::Worker::run,this);
}



ThreadPool::Worker::~Worker(){

    if(_active){
        stop();
    }
    _callbacks_cv.notify_one(); //唤醒，才会继续判断_active条件
    _thread.join();

}

void ThreadPool::Worker::enqueue(function<void()> cb){
	_lock.lock();

	_callbacks.push_back(cb);

    _callbacks_cv.notify_one();   //关于unlock与wakeup之间的顺序也是一个有趣的话题

	_lock.unlock();
}


void ThreadPool::Worker::stop(){

	_active = false;
}



// the stl containers are not at all thread safe , so you have to lock
// where you are manipulating them even if you know that you are the only
// one puling stuff out of them, and that you are only pulling stuff out
// that is already there.
void* ThreadPool::Worker::run(void* arg){
	Worker *worker = (Worker*)arg;

    pthread_setspecific(G_WORKER_KEY, arg);

    std::unique_lock<mutex> cond_lock(worker->_lock);
	while(worker->_active){
        while(!worker->_callbacks.empty() && worker->_active){

			function<void()> fn = worker->_callbacks.front();
			worker->_callbacks.pop_front();  //自动析构
            fn();
		}
        worker->_callbacks_cv.wait(cond_lock);   //release the mutex,and wait will be blocked
	}
	return 0;
}



