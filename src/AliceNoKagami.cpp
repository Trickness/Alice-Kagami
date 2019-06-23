#include "include/AliceNoKagami.hpp"
#include "include/Bangumi/BangumiAdaptor.hpp"
#include <curl/curl.h>
using namespace std;

void testCallback(int taskID,Wonderland::Status status, void* Data, size_t t){
    DEBUG_MSG((char*)Data);
    DEBUG_MSG(taskID);
    free(Data);
}

AliceNoKagami::AliceNoKagami(){

    curl_global_init(CURL_GLOBAL_DEFAULT);      // should only init once
    this->mAdaptors.push_back(new BangumiAdaptor());
    DEBUG_MSG("Alice's Kagami Version 0.1 loaded");
}

AliceNoKagami::~AliceNoKagami(){
    if(!this->mAdaptors.empty()){
        for(auto i:this->mAdaptors)
            delete i;
    }
}

std::string AliceNoKagami::GetParsedContentSync(const char* URI, Wonderland::CachePolicy Policy){
    for(auto adaptor:this->mAdaptors){
        void* Buffer = nullptr;
        if(adaptor->CheckURI(URI)){
            size_t bytes = adaptor->GetParsedSync(URI, Buffer, Policy);
            if(bytes == 0){
                DEBUG_MSG("Failed to parse URI [" << URI << "]");
                return "";
            }
            return std::string((char*)Buffer);
        }
    }
    return "";
}

void AliceNoKagami::GetParsedContentAsync(const char* URI, Wonderland::CachePolicy Policy, Wonderland::ParserCallback _Callback){
    for(auto adaptor:this->mAdaptors){
        if(adaptor->CheckURI(URI)){
            adaptor->GetParsedAsync(URI, Policy, _Callback);
            return;
        }
    }
    DEBUG_MSG("Warning: No Adaptor found for [" << URI << "]");
}

size_t AliceNoKagami::GetHTMLSync(const char* URI, void *&Buffer,  Wonderland::CachePolicy Policy){
    for(auto adaptor:this->mAdaptors){
        if(adaptor->CheckURI(URI)){
            size_t bytes = adaptor->GetHTMLSync(URI,Buffer, Policy);
            if(bytes == 0){
                DEBUG_MSG("Failed to GET URI [" << URI << "]");
                return 0;
            }
            return bytes;
        }
    }
    return 0;
}

void AliceNoKagami::GetHTMLAsync(const char* URI, Wonderland::CachePolicy Policy, Wonderland::NetworkCallback _Callback){
    for(auto adaptor:this->mAdaptors){
        if(adaptor->CheckURI(URI)){
            adaptor->GetHTMLAsync(URI, Policy, _Callback);
            return;
        }
    }
    DEBUG_MSG("Warning: No Adaptor found for [" << URI << "]");
    return ;
}

