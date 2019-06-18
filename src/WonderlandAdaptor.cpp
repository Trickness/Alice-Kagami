#include "include/WonderlandAdaptor.hpp"
#include <boost/asio.hpp>
#include <vector>
#include <curl/curl.h>

using namespace std;

WonderlandAdaptor::WonderlandAdaptor(){
    this->mCastBook = new CastBook();
    DEBUG_MSG("Wonderland Adaptor Version 0.1 is loaded.");
}

WonderlandAdaptor::~WonderlandAdaptor(){
    delete this->mCastBook;
}

size_t WonderlandAdaptor::GetResource(const char* URI,  \
        void *&Buffer,      \
        WonderlandAdaptor::CachePolicy CachePolicy,     \
        WonderlandAdaptor::Callback _Callback){
    CHECK_PARAM_PTR(URI,0)
    if(CachePolicy == FIRST_FROM_CACHE || CachePolicy == ONLY_FROM_CACHE){
        unique_ptr<CastBookRecord> result = this->mCastBook->GetRecord(URI);
        if(result){
            Buffer = malloc(result->GetSize());
            memcpy(Buffer, result->GetData(), result->GetSize());
            return result->GetSize();
        }
        if(CachePolicy == ONLY_FROM_CACHE){
            return 0;
        }
    }
    this->mThreadPool.AddTask(std::bind(&WonderlandAdaptor::NetworkTask,this,_Callback));
    return 0;
}

void WonderlandAdaptor::NetworkTask(WonderlandAdaptor::Callback _Callback){
            CastBook* castBook = new CastBook();   // new sqlite session
            CURL *curl = curl_easy_init();                      // new curl_session;
            curl_easy_cleanup(curl);
            delete castBook;
            if(_Callback != nullptr)
                _Callback(Status::FAILED,"Error1234",10);
        }

void WonderlandAdaptor::PutResource(const char* URI, const void *Buffer, size_t Bytes){
    CHECK_PARAM_PTR(URI,)
    CHECK_PARAM_PTR(Buffer,)
    this->mCastBook->PutRecord(URI,Buffer,Bytes);
}