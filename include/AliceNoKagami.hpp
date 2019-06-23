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
        size_t GetHTMLSync(const char* URI, void *&Buffer,  Wonderland::CachePolicy Policy = Wonderland::CachePolicy::FIRST_FROM_CACHE);
        void GetParsedContentAsync(const char* URI, Wonderland::CachePolicy Policy = Wonderland::CachePolicy::FIRST_FROM_CACHE, Wonderland::ParserCallback _Callback = nullptr);
        void GetHTMLAsync(const char* URI, Wonderland::CachePolicy Policy = Wonderland::CachePolicy::FIRST_FROM_CACHE, Wonderland::NetworkCallback _Callback = nullptr);
    private:
        std::list<WonderlandAdaptor*> mAdaptors;
};  

#endif
