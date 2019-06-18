#ifndef _WONDERLAND_ADAPTOR_
#define _WONDERLAND_ADAPTOR_

#include <iostream>
#include <functional>
#include <curl/curl.h>
#include "Config.hpp"
#include "CastBook.hpp"
#include "ThreadPool.hpp"

/*  Network Connector & Cache controller */

class WonderlandAdaptor{
    public:
        enum CachePolicy{ONLY_FROM_CACHE, FIRST_FROM_CACHE, NEVER_FROM_CACHE};    
        enum Status{SUCCESS, FAILED};
        //                        return_status                 data        data_size
        typedef std::function<void(WonderlandAdaptor::Status, const char*, size_t)> Callback; 
        WonderlandAdaptor();
        ~WonderlandAdaptor(); 
        
        /* Buffer need to be a void ptr, and you should free() it yourself */
        size_t GetResource( \
            const char* URI, \
            void *&Buffer,WonderlandAdaptor::CachePolicy Policy = ONLY_FROM_CACHE,    \
            WonderlandAdaptor::Callback _Callback = nullptr   \
        );
        void   PutResource(const char* URI, const void *Buffer, size_t Bytes);

        void NetworkTask(WonderlandAdaptor::Callback _Callback);
        void CallbackTest(WonderlandAdaptor::Status, const char* , size_t);
    private:
        CastBook *mCastBook;
        ThreadPool mThreadPool;
}; 

#endif