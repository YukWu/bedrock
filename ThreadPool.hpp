#ifndef THREADPOOL_BEDROCK_H
#define THREADPOOL_BEDROCK_H

#include <algorithm>
#include <list>
#include <vector>
#include <functional>
#include <thread>
#include <mutex>
#include <thread>
#include <memory>
#include <atomic>
#include <condition_variable>

#include <assert.h>


namespace bedrock
{
	extern pthread_key_t  G_WORKER_KEY;

	/**
     * Provides an easy way to execute assorted functions
     * in the context of a thread pool.
     */
    class ThreadPool
    {
    public:
        ThreadPool(unsigned int workers);
        ~ThreadPool();

        int  enqueue(std::function<void()> cb);
        void enqueue(std::function<void()> cb,int id);    //指定执行线程的id


        int  getNextThreadID(void);

        static ThreadPool* getCurrentThreadPool();
        static int getCurrentThreadKey();
        void waitForAllEnd(std::function<void()> cb);
        void waitForAllEndBlock(std::function<void()> cb);
    private:
        /*
         工作线程封装类
         */
        class Worker
        {
            public:
                Worker(ThreadPool* owner, int id);
                ~Worker();

                void enqueue(std::function<void()> cb);



            protected:


                static void*          run(void* arg);
                void stop();

            public:


                ThreadPool*                        _owner;

                int                                _threadkey; //线程key
                std::mutex                         _lock ;
                std::list<std::function<void()> >  _callbacks; //任务队列
                std::condition_variable            _callbacks_cv; //条件变量
                std::thread                        _thread;
                std::atomic<bool>                  _active; //线程是否运行中

        };
    private:
        std::vector<std::unique_ptr<Worker> > _workers;         //保存工作线程

        unsigned int         _current_worker;  //当前指向的工作线程号
        std::mutex           _current_worker_lock;
    };

    inline void _findPoolAndKey(ThreadPool *& pool, int &key)
    {
        bool pool_defined = (pool != nullptr);
        if (!pool_defined)
        {
            // If a NULL thread pool is provided, try and get the "current" one
            pool = ThreadPool::getCurrentThreadPool();
            assert(pool != nullptr);
        }

        if (key == -1)
        {
            // if the calling function didn't pass in a threadpool, we use the current one
            if (!pool_defined)
            {
                key = ThreadPool::getCurrentThreadKey();
            }
            // otherwise, we round-robin
            else
            {
                key = pool->getNextThreadID();
            }
            assert(key != -1);
        }
    }
};
#endif //THREADPOOL_BEDROCK_H
