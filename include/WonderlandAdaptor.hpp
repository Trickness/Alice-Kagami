#ifndef _WONDERLAND_ADAPTOR_
#define _WONDERLAND_ADAPTOR_

#include <iostream>
#include <functional>
#include <curl/curl.h>
#include "Config.hpp"
#include "CastBook.hpp"
#include "ThreadPool.hpp"

/*  Network Connector & Cache controller */
/*  Should only one instance in the program */

class WonderlandAdaptor{
    public:
        enum CachePolicy{ONLY_FROM_CACHE, FIRST_FROM_CACHE, NEVER_FROM_CACHE};    
        enum Status{SUCCESS, FAILED};
        //                        return_status              data     data_size     !data should be free() manualy
        typedef std::function<void(WonderlandAdaptor::Status, void*, size_t)> Callback; 
        WonderlandAdaptor();
        ~WonderlandAdaptor(); 
        
        
        size_t GetResourceAnsync( \
            const char* URI, \
            WonderlandAdaptor::CachePolicy Policy = FIRST_FROM_CACHE,    \
            WonderlandAdaptor::Callback _Callback = nullptr   \
        );

        /* Buffer need to be a void ptr, and you should free() it yourself */
        size_t GetResourceSync( \
            const char* URI, \
            void *&Buffer,  \
            WonderlandAdaptor::CachePolicy Policy = FIRST_FROM_CACHE      \
        );
        void   PutResource(const char* URI, const void *Buffer, size_t Bytes);

        void NetworkTask(const char*, WonderlandAdaptor::Callback _Callback);
        void CallbackTest(WonderlandAdaptor::Status, const char* , size_t);
    protected:
        WonderlandAdaptor(const WonderlandAdaptor &);
        const WonderlandAdaptor operator=(const WonderlandAdaptor&);
    private:
        CastBook *mCastBook;
        ThreadPool mThreadPool;
}; 

#endif