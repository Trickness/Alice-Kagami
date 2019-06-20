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
    DEBUG_MSG("Wonderland Adaptor Version 0.1 is loaded.");
}

WonderlandAdaptor::~WonderlandAdaptor(){
    delete this->mCastBook;
}

size_t WonderlandAdaptor::GetHTMLAsync(const char* URI,  \
        Wonderland::CachePolicy CachePolicy,     \
        Wonderland::NetworkCallback _Callback){
    CHECK_PARAM_PTR(URI,0)
    char* copyURI = (char*)malloc(strlen(URI)+1);
    memset(copyURI,0,strlen(URI)+1);
    strcpy(copyURI,URI);
    this->mThreadPool.AddTask(std::bind(&WonderlandAdaptor::NetworkTask,this,copyURI,CachePolicy, _Callback));
    return 0;
}

size_t WonderlandAdaptor::GetHTMLSync(const char* URI,  \
        void *&Buffer,      \
        Wonderland::CachePolicy CachePolicy){
    CHECK_PARAM_PTR(URI,0)
    CastBook *castBook = new CastBook();
    if(CachePolicy == Wonderland::CachePolicy::FIRST_FROM_CACHE || CachePolicy == Wonderland::CachePolicy::ONLY_FROM_CACHE){
        unique_ptr<CastBookRecord> result = castBook->GetRecord(URI);
        if(result){
            Buffer = malloc(result->GetSize());
            memcpy(Buffer, result->GetData(), result->GetSize());
            return result->GetSize();
        }
        if(CachePolicy == Wonderland::CachePolicy::ONLY_FROM_CACHE){
            return 0;
        }
    }
    size_t bytes = 0;
    Wonderland::Status status = this->NetworkFetch(URI, Buffer, bytes);
    if(status == Wonderland::Status::SUCCESS)
        castBook->PutRecord(URI, Buffer, bytes);
    return bytes;
}


void WonderlandAdaptor::NetworkTask(const char* URI, Wonderland::CachePolicy Policy, Wonderland::NetworkCallback _Callback){
    void* Buffer = nullptr;
    size_t bytes = this->GetHTMLSync(URI, Buffer, Policy);
    Wonderland::Status status ;
    if(bytes > 0)
        status = Wonderland::Status::SUCCESS;
    else
        status = Wonderland::Status::FAILED;
    delete URI;
    if(_Callback != nullptr)
        _Callback(status,Buffer,bytes);
    if(Buffer != nullptr)
        free(Buffer);
}


Wonderland::Status WonderlandAdaptor::NetworkFetch(const char* URI, void *&Buffer, size_t &bytes){
    CURL *curl = curl_easy_init();          // new curl_session;
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
        }
        curl_easy_cleanup(curl);
        return Wonderland::Status::SUCCESS;
    }else{
        return Wonderland::Status::FAILED;
    }
}

void WonderlandAdaptor::CacheResource(const char* URI, const void *Buffer, size_t Bytes){
    CHECK_PARAM_PTR(URI,)
    CHECK_PARAM_PTR(Buffer,)
    this->mCastBook->PutRecord(URI,Buffer,Bytes);
}

size_t WonderlandAdaptor::GetParsedSync( \
            const char* URI, \
            void *&Buffer,  \
            Wonderland::CachePolicy Policy){
    CHECK_PARAM_PTR(URI,0);
    size_t bytes = this->GetHTMLSync(URI, Buffer, Policy);
    if(bytes){
        std::string result = this->ParseContent(Buffer, bytes);
        if(result.length() != 0){
            bytes = result.length();
            Buffer = (char*) malloc(bytes+1);
            memset(Buffer,0,bytes+1);
            memcpy(Buffer, result.c_str(), bytes);
        }else{
            bytes = 0;
        }
    }
    return bytes;
}

size_t WonderlandAdaptor::GetParsedAsync(const char* URI,  \
        Wonderland::CachePolicy CachePolicy,     \
        Wonderland::ParserCallback _Callback){
    CHECK_PARAM_PTR(URI,0)
    char* copyURI = (char*)malloc(strlen(URI)+1);
    memset(copyURI,0,strlen(URI)+1);
    strcpy(copyURI,URI);
    this->mThreadPool.AddTask(std::bind(&WonderlandAdaptor::ParseTask,this,copyURI,CachePolicy,_Callback));
    return 0;
}

void WonderlandAdaptor::ParseTask(const char* URI, Wonderland::CachePolicy Policy, Wonderland::ParserCallback _Callback){
    void* Buffer = nullptr;
    size_t bytes = this->GetParsedSync(URI,Buffer, Policy);
    Wonderland::Status status ;
    if(bytes > 0)
        status = Wonderland::Status::SUCCESS;
    else
        status = Wonderland::Status::FAILED;
    delete URI;
    if(_Callback != nullptr)
        _Callback(status,Buffer,bytes);
    if(Buffer != nullptr)
        free(Buffer);
    delete URI;
}