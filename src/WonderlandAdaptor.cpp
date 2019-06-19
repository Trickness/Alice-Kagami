#include "include/WonderlandAdaptor.hpp"
#include <boost/asio.hpp>
#include <string>
#include <vector>
#include <curl/curl.h>

using namespace std;

size_t writeToBuffer(void *ptr, size_t size, size_t count, void *buffer){
    ((string*)buffer)->append((char*)ptr,0,size * count);
    return size * count;
}

WonderlandAdaptor::WonderlandAdaptor(){
    this->mCastBook = new CastBook();
    curl_global_init(CURL_GLOBAL_DEFAULT);      // should only init once
    DEBUG_MSG("Wonderland Adaptor Version 0.1 is loaded.");
}

WonderlandAdaptor::~WonderlandAdaptor(){
    delete this->mCastBook;
}

size_t WonderlandAdaptor::GetResourceAnsync(const char* URI,  \
        WonderlandAdaptor::CachePolicy CachePolicy,     \
        WonderlandAdaptor::Callback _Callback){
    CHECK_PARAM_PTR(URI,0)
    char* copyURI = (char*)malloc(strlen(URI)+1);
    memset(copyURI,0,strlen(URI)+1);
    strcpy(copyURI,URI);
    this->mThreadPool.AddTask(std::bind(&WonderlandAdaptor::NetworkTask,this,copyURI,_Callback));
    return 0;
}

size_t WonderlandAdaptor::GetResourceSync(const char* URI,  \
        void *&Buffer,      \
        WonderlandAdaptor::CachePolicy CachePolicy){
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
    CURL *curl = curl_easy_init();          // new curl_session;
    size_t bytes = 0;
    if(curl){
        string data;
        curl_easy_setopt(curl, CURLOPT_URL, URI);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeToBuffer);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
        CURLcode res = curl_easy_perform(curl);
        if(res != CURLE_OK){
            bytes = strlen(curl_easy_strerror(res));
            Buffer = malloc(bytes+1);
            memset(Buffer,0,bytes+1);
            strcpy((char*)Buffer,curl_easy_strerror(res));
        }else{
            bytes = data.length();
            Buffer= malloc(bytes+1);
            memset(Buffer, 0, bytes+1);
            memcpy(Buffer, data.c_str(), bytes);
            this->mCastBook->PutRecord(URI,Buffer,bytes);
        }
        curl_easy_cleanup(curl);
        return bytes;
    }else{
        return 0;
    }
}

void WonderlandAdaptor::NetworkTask(const char* URI, WonderlandAdaptor::Callback _Callback){
            CastBook* castBook = new CastBook();    // new sqlite session
            CURL *curl = curl_easy_init();          // new curl_session;
            void* Buffer = nullptr;
            size_t bytes = 0;
            WonderlandAdaptor::Status status;
            if(curl){
                string data;
                curl_easy_setopt(curl, CURLOPT_URL, URI);
                curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeToBuffer);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
                CURLcode res = curl_easy_perform(curl);
                if(res != CURLE_OK){
                    bytes = strlen(curl_easy_strerror(res));
                    Buffer = malloc(bytes+1);
                    memset(Buffer,0,bytes+1);
                    strcpy((char*)Buffer,curl_easy_strerror(res));
                    status = Status::FAILED;
                }else{
                    bytes = data.length();
                    Buffer= malloc(bytes+1);
                    memset(Buffer, 0, bytes+1);
                    memcpy(Buffer, data.c_str(), bytes);
                    status = Status::SUCCESS;
                    castBook->PutRecord(URI,Buffer,bytes);
                }
                curl_easy_cleanup(curl);
            }
            delete URI;
            delete castBook;
            if(_Callback != nullptr)
                _Callback(Status::FAILED,Buffer,bytes);
        }

void WonderlandAdaptor::PutResource(const char* URI, const void *Buffer, size_t Bytes){
    CHECK_PARAM_PTR(URI,)
    CHECK_PARAM_PTR(Buffer,)
    this->mCastBook->PutRecord(URI,Buffer,Bytes);
}