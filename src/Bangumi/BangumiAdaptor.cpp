#include "include/Bangumi/BangumiAdaptor.hpp"
#include "src/third-party/gumbo-parser/GumboQuery.h"

BangumiAdaptor::BangumiAdaptor()
    :WonderlandAdaptor(){

}

BangumiAdaptor::~BangumiAdaptor(){

}

bool BangumiAdaptor::CheckURI(const char* URI) const {
    return true;
}

std::string BangumiAdaptor::ParseContent(void* Data, size_t Bytes) const{
    if(Bytes > 0)
        DEBUG_MSG((char*)Data);
    return "TEST RETURN";
}