#ifndef _WONDERLAND_ADAPTOR_
#define _WONDERLAND_ADAPTOR_

#include <iostream>
#include "Config.hpp"
#include "CastBook.hpp"

/*  Network Connector & Cache controller */
namespace Wonderland{
    static short ONLY_FROM_CACHE = 1;
    static short FIRST_FROM_CACHE = 2;
    static short NEVER_FROM_CACHE = 3; 
};
class WonderlandAdaptor{
    public:
        WonderlandAdaptor();
        ~WonderlandAdaptor(); 
        
        /* Buffer need to be a void ptr, and you should free() it yourself */
        size_t GetResource(const char* URI, void *&Buffer, short CacheStratagy = Wonderland::FIRST_FROM_CACHE);


        void   PutResource(const char* URI, const void *Buffer, size_t Bytes);
    private:
        CastBook *mCastBook;
}; 

#endif