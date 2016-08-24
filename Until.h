//
//  Until.h
//  
//
//  Created by Lionel on 16/8/14.
//
//

#pragma once

#include <errno.h>

namespace bedrock
{
    //wrap call to f(args) in loop to retry on EINTR
    template<typename F,typename... Args>
    ssize_t wrapNoInt(F f,Args... args){
        ssize_t r;
        do
        {
            r = f(args...);
        }while(r==-1 && errno==EINTR);
        return r;
    }
}
