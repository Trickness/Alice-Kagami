#ifndef _BANGUMI_ADAPTOR_HPP_
#define _BANGUMI_ADAPTOR_HPP_
/* Bangumi.tv & bgm.tv */

#include "include/WonderlandAdaptor.hpp"

class BangumiAdaptor : public WonderlandAdaptor{
    public:
        BangumiAdaptor();
        ~BangumiAdaptor();
        
        bool CheckURI(const char* URI) const;
        std::string ParseContent(void* , size_t) const;
};

#endif