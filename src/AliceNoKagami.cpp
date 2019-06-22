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
    /* 
    this->mAdaptor = new BangumiAdaptor();
    void* Buffer = nullptr;
    size_t bytes = this->mAdaptor->GetParsedSync("https://bgm.tv/subject/276",  \
                                    Buffer, \
                                    Wonderland::CachePolicy::FIRST_FROM_CACHE   \
                                    );
                                    //std::bind(testCallback,1,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
    if(Buffer != nullptr){
        DEBUG_MSG((char*)Buffer);
        free(Buffer);
    } */
    
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


