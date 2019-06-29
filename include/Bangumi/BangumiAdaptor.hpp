#ifndef _BANGUMI_ADAPTOR_HPP_
#define _BANGUMI_ADAPTOR_HPP_
/* Bangumi.tv & bgm.tv */

#include "include/WonderlandAdaptor.hpp"

#define _BGM_PROTOCOL_ "https:"
#define _BGM_DOMAIN_   "bgm.tv"

class BangumiAdaptor : public WonderlandAdaptor{
    public:
        BangumiAdaptor();
        ~BangumiAdaptor();
        
        bool CheckURI(const std::string &URI) const;
    protected:
        std::string ParseContent(std::string URI, const std::string &Data) const;
};

#endif