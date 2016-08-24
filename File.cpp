//
//  File.cpp
//  
//
//  Created by Lionel on 16/8/11.
//
//
#include <stdexcept>

#include "File.h"
#include "Exception.h"
#include "Until.h"

namespace bedrock
{
    //默认构造函数,尚未完成初始化
    File::File():fd_(-1),ownsFd_(false){}


    File::File(int fd,bool ownFd):fd_(fd),ownsFd_(ownFd)
    {
        /* TODO: did some check about the fd and ownFd */
        CHECK_THROW(fd_!=-1 ||(fd_==-1 && ownsFd_==false) ,std::invalid_argument); //表达式为假时，抛出异常
    }
    File::File(const char* name,int flags,mode_t mode):fd_(::open(name,flags,mode)),ownsFd_(false)
    {
        if( fd_ == -1)
        {
            /* throw some exception*/
            throwSystemError("open file fail"); //这里可以用字符串插值则甚好，如C#的format插值
        }
        ownsFd_ = true;
    }

    //C++11 构造函数委托
    File::File(std::string&& name,int flags,mode_t mode)
    :File(name.c_str(),flags,mode){}

    //moveable
    File::File(File&& other) noexcept
    :fd_(other.fd_),ownsFd_(other.ownsFd_)
    {
        //此处不用closeNothrow的原因是这里是构造函数，File的资源还不存在，不用释放
        other.release(); //把other的值置为原始状态，资源并没有释放
    }

    File& File::operator=(File&& other)
    {
        closeNoThrow(); //释放原来的文件资源
        swap(other);
        return *this;
    }

    File::~File()
    {
        auto fd = fd_;
        if(!closeNoThrow()) // ignore most error
        {
            throwSystemError("closeing fd error");
        }
    }
    
    /*
      make file with tmpfile()
	 tmpfile()返回后会马上把文件连接删除，但是文件依然存在于内存中
	 所以，在TMPDIR文件夹中不会看到该文件。lsof应该可以看到
     tmpfile 在$TMPDIR(默认为/tmp)文件夹内创建一个以二进制更新模式(wb+)的文件。
     被创建的临时文件会在流关闭的时候、或者在程序终止时自动删除
     */
    File File::temprary()
    {
        FILE* tmpFile = tmpfile();
        checkFopenError(tmpFile,"tmpfile() fail");
        int fd = ::dup(fileno(tmpFile)); //复制文件描述符
        checkUnixError(fd,"dup() fail");
        fclose(tmpFile);//因为dup了一份，这里fclose不会导致文件被关闭
        return File(fd,true);
    }
    
    int File::release() noexcept //变为默认值
    {
        int released = fd_;
        fd_ = -1;
        ownsFd_ = false;
        return released;
    }
    
    void File::swap(File& other)
    {
        using std::swap;
        swap(fd_,other.fd_);
        swap(ownsFd_,other.ownsFd_);
    }
    
    void swap(File& a,File& b)
    {
        a.swap(b);
    }
    
    File File::dup() const
    {
        if(fd_!=-1)
        {
            int fd = ::dup(fd_);
            checkUnixError(fd,"dup() failed");
            return File(fd,true);
        }
        return File();
    }
    
    void File::close()
    {
        if(!closeNoThrow())
        {
            //throw some exception
            throwSystemError("close() failed");
        }
    }
    bool File::closeNoThrow() //关闭发生错误，不抛出异常，即close -1时也不报错
    {
        int r = ownsFd_ ? ::close(fd_):0;
        release();
        return r==0;
    }
    
    void File::lock()
    {
        doLock(LOCK_EX);
    }
    
    bool File::try_lock()
    {
        return doTryLock(LOCK_EX);
    }
    
    void File::lock_shared()
    {
        doLock(LOCK_SH);
    }
    
    bool File::try_lock_shared()
    {
        return doTryLock(LOCK_SH);
    }
    
    void File::doLock(int op)
    {
        //check tool
        checkUnixError(wrapNoInt(::flock,fd_,op),"flock() failed (doLock)");
    }
    
    bool File::doTryLock(int op)
    {
        int r = wrapNoInt(::flock,fd_,op|LOCK_NB);
        // flock returns EWOULDBLOCK if already locked
        if( r==-1 && errno==EWOULDBLOCK) return false;
        checkUnixError(r,"flock() failed (doTryLock)");
        return true;
    }
    
    void File::unlock()
    {
        checkUnixError(wrapNoInt(flock,fd_,LOCK_UN),"flock() failed (unlock)");
    }
    
    void File::unlock_shared()
    {
        unlock();
    }
    
};

