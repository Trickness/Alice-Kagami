#ifndef _ALICE_NO_KAGAMI_
#define _ALICE_NO_KAGAMI_

#include <iostream>
#include "Config.hpp"
#include "Bangumi/BangumiAdaptor.hpp"

class AliceNoKagami{
    public:
        AliceNoKagami();
        ~AliceNoKagami();
    private:
        BangumiAdaptor *mAdaptor;
};

#endif
