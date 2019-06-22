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

namespace Wonderland{
    enum CachePolicy{ONLY_FROM_CACHE, FIRST_FROM_CACHE, NEVER_FROM_CACHE};    
    enum Status{SUCCESS, FAILED};
    //      return_status              data     data_size     // data will be free() after this function return
    typedef std::function<void(Status, void* data, size_t)> NetworkCallback; 
    typedef std::function<void(Status, void* data, size_t)> ParserCallback;
}

class WonderlandAdaptor{
    public:
        WonderlandAdaptor();
        ~WonderlandAdaptor(); 
        
        #ifdef THREAD_POOL_SIZE
        size_t GetHTMLAsync( \
            const char* URI, \
            Wonderland::CachePolicy Policy = Wonderland::CachePolicy::FIRST_FROM_CACHE,    \
            Wonderland::NetworkCallback _Callback = nullptr   \
        );
        #endif

        /* Buffer need to be a void ptr, and you should free() it yourself (thread safe)*/
        size_t GetHTMLSync( \
            const char* URI, \
            void *&Buffer,  \
            Wonderland::CachePolicy Policy = Wonderland::CachePolicy::FIRST_FROM_CACHE      \
        );
        virtual bool CheckURI(const char* URI) const = 0;
        virtual std::string ParseContent(const char* ,const void* , size_t) const = 0;

        #ifdef THREAD_POOL_SIZE
        size_t GetParsedAsync(  \
            const char* URI, \
            Wonderland::CachePolicy Policy = Wonderland::CachePolicy::FIRST_FROM_CACHE,    \
            Wonderland::ParserCallback _Callback = nullptr   \
        );
        #endif

        /* Buffer need to be a void ptr, and you should free() it yourself, (thread safe) */
        size_t GetParsedSync( \
            const char* URI, \
            void *&Buffer,  \
            Wonderland::CachePolicy Policy = Wonderland::CachePolicy::FIRST_FROM_CACHE      \
        );
        void   CacheResource(const char* URI, const void *Buffer, size_t Bytes);

    protected:
        WonderlandAdaptor(const WonderlandAdaptor &);
        void NetworkTask(const char*URI, Wonderland::CachePolicy, Wonderland::NetworkCallback _Callback);
        Wonderland::Status NetworkFetch(const char* URI, void *&Buffer, size_t &bytes);
        void ParseTask(const char*,Wonderland::CachePolicy, Wonderland::ParserCallback _Callback);
    private:
        CastBook *mCastBook;
    #if THREAD_POOL_SIZE != 0
        ThreadPool mThreadPool;
    #endif
}; 

#endif