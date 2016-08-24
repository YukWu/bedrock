//
//  Exception.h
//  
//
//  Created by Lionel on 16/8/13.
//
//

#pragma once

#include <errno.h>
#include <stdio.h>

#include <stdexcept>
#include <system_error>

namespace bedrock
{
    /* 为C API 抛出异常的各种帮助类 */
    
    /* system_error的封装类，主动抛出一个系统错误 */
    /* [[noreturn]]为C++11的新特性，表示函数不能返回，要求函数不可执行到函数末尾且函数声明的返回值类型必须为void */
    /* std::system_error 是专门为了系统调用出错而设计的异常类 
       std::system_category 表示使用系统错误码与错误message的映射。
       system_error的code().message()获取错误message
       std::system_error的第三个参数是字符串，用于作进一步解析,由what()返回
     */
    [[noreturn]] inline void throwSystemErrorExplict(int err,const char* msg)
    {
        throw std::system_error(err,std::system_category(),msg);
    }
    
    [[noreturn]] void throwSystemErrorExplict(int err,std::string&& msg)
    {
        throwSystemErrorExplict(err,msg.c_str());
    }
    /* std::forward的作用，用于左右值转换，应用在完美转发 */
    /* 例子：
            s 是左值时（s是为右值时，似乎不能用forward转为左值）
            但是要注意到，当一个函数的参数是右值引用时，函数内部的这个参数会变成左值
            std::forward<string>(s) 把s转为右值
             std::forward<string&>(s) 把s转为左值
     */
    /* 此处暂时不用，不是很懂为什么要用模板
    template <typename... Args>
    [[noreturn]] void throwSystemErrorExplict(int err,Args&&... args)
    { 
        throwSystemErrorExplict(err,std::string("").c_str());
    }
     */
    
    
    
    /* 由errno触发 的 异常 helper类 */
    /*template <typename... Args>
    [[noreturn]] void throwSystemError(Args&&... args)
    {
        throwSystemErrorExplict(errno,std::forward<Args>(args)...);
    }
    */
    [[noreturn]] void throwSystemError(std::string&& args)
    {
        throwSystemErrorExplict(errno,args.c_str());
    }
    
    [[noreturn]] void throwSystemError(const char* args)
    {
        throwSystemErrorExplict(errno,args);
    }
    
    //检查POSIX 调用的返回值，0是成功，否则返回错误码
    void checkPosixError(int err,std::string&& args) //太累了，暂时只定右值参数的类型
    {
        if(err!=0)
            throwSystemErrorExplict(err,args.c_str());
    }
    
    //检查linux 内核风格调用的返回值，（>=0 为成功，负数的表示错误)
    void checkKernelError(ssize_t ret,std::string&& args)
    {
        if(ret<0)
            throwSystemErrorExplict(-ret,args.c_str()); //why need negative ret?
    }
    
    //检查传统 Unix 风格调用的返回值，-1为错误，错误码为errno
    void checkUnixError(ssize_t ret,std::string&& args)
    {
        if(ret==-1)
            throwSystemError(args.c_str());
    }
    
    void checkUnixErrorExplicit(ssize_t ret,int err,std::string&& args)
    {
        if(ret==-1)
            throwSystemErrorExplict(err,args.c_str());
    }
    
    //检查标准文件打开函数的返回值
    //包括fopen fdopen freopen tmpfile等返回FILE*的函数
    void checkFopenError(FILE* fp,std::string&& args)
    {
        if(!fp)
            throwSystemError(args.c_str());
    }
    
    void checkFopenErrorExplicit(FILE* fp,int err,std::string&& arg)
    {
        if(!fp)
            throwSystemErrorExplict(err,arg.c_str());
    }
    
    //异常版ASSERT 值检查错误，抛出异常
    template <typename E,typename V>
    void throwOnFail(V&& value,std::string&& arg)
    {
        if(!value)
            throw E(arg.c_str());
    }
    
    //宏版本，针对V为布尔表达式,并可自定义抛出的异常种类
#define CHECK_THROW(cond,E) \
    bedrock::throwOnFail<E>((cond),"Check Failed: " #cond)
}
