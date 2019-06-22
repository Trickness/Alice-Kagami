#ifndef _ALICE_NO_KAGAMI_
#define _ALICE_NO_KAGAMI_

#include <iostream>
#include <list>
#include "Config.hpp"
#include "WonderlandAdaptor.hpp"


class AliceNoKagami{
    public:
        AliceNoKagami();
        ~AliceNoKagami();
        std::string GetParsedContentSync(const char* URI, Wonderland::CachePolicy Policy = Wonderland::CachePolicy::FIRST_FROM_CACHE);
    private:
        std::list<WonderlandAdaptor*> mAdaptors;
};  

#endif
