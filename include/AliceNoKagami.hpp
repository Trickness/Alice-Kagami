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
        std::string GetParsedContentSync(std::string URI, Wonderland::CachePolicy Policy = Wonderland::CachePolicy::FIRST_FROM_CACHE);
        std::string GetHTMLSync(std::string URI,  Wonderland::CachePolicy Policy = Wonderland::CachePolicy::FIRST_FROM_CACHE);
        void GetParsedContentAsync(std::string URI, Wonderland::CachePolicy Policy = Wonderland::CachePolicy::FIRST_FROM_CACHE, Wonderland::NetworkCallback _Callback = nullptr);
        void GetHTMLAsync(std::string URI, Wonderland::CachePolicy Policy = Wonderland::CachePolicy::FIRST_FROM_CACHE, Wonderland::NetworkCallback _Callback = nullptr);
    private:
        std::list<WonderlandAdaptor*> mAdaptors;
};  

#endif
