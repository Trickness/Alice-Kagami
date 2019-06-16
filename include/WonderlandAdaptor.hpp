#ifndef _WONDERLAND_ADAPTOR_
#define _WONDERLAND_ADAPTOR_

#include <iostream>
#include "Config.hpp"
#include "CastBook.hpp"

/*  Network Connector & Cache controller */

class WonderlandAdaptor{
    public:
        WonderlandAdaptor();
        ~WonderlandAdaptor(); 
        
    private:
        CastBook *mCastBook;
}; 

#endif