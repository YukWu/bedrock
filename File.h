#pragma once

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>

namespace bedrock{

class File
{
 public:
    File();
    explicit File(int fd,bool ownFd=false);

    explicit File(const char* name,int flags=O_RDONLY,mode_t mode=0666);
    explicit File(std::string&& name,int flags=O_RDONLY,mode_t mode=0666);

    ~File();

    static File temprary(); // create tmp file

    int fd() const { return fd_ ;}

    //C++11 特性，explicit operator bool() safety bool idtom
    explicit operator bool() const
    {
	   return fd_!=-1;
    }
    
    File dup() const ;
    
    void close();
    
    void swap(File& other);
    
    //movable
    File(File&&) noexcept;
    File& operator=(File&&);
    // 文件锁，用于进程间同步
    void lock();
    bool try_lock();
    void unlock();
    
    void lock_shared();
    bool try_lock_shared();
    void unlock_shared();
    
private:
    void doLock(int op);
    bool doTryLock(int op);
    int release() noexcept;
    bool closeNoThrow();
    
    //unique，不可复制
    File(const File&) = delete;
    File& operator=(const File&) = delete;
    
    int fd_;
    bool ownsFd_;
};

    void swap(File& a,File& b);
}
