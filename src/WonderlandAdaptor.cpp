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
}

WonderlandAdaptor::~WonderlandAdaptor(){
    delete this->mCastBook;
}


void WonderlandAdaptor::GetHTMLAsync(const char* URI,  \
        Wonderland::CachePolicy CachePolicy,     \
        Wonderland::NetworkCallback _Callback){
#if THREAD_POOL_SIZE != 0
    CHECK_PARAM_PTR(URI,)
    char* copyURI = (char*)malloc(strlen(URI)+1);
    memset(copyURI,0,strlen(URI)+1);
    strcpy(copyURI,URI);
    this->mThreadPool.AddTask(std::bind(&WonderlandAdaptor::NetworkTask,this,copyURI,CachePolicy, _Callback));
#endif
    return;
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
    string Domain = this->GetDomainFromURI(URI);
    string Cookies = castBook->GetRosterCookies(Domain.c_str());

    Wonderland::Status status = this->NetworkFetch(URI,Cookies, Buffer, bytes);
    if(status == Wonderland::Status::SUCCESS){
        castBook->PutRecord(URI, Buffer, bytes);
        castBook->SetRosterCookies(Domain.c_str(), Cookies.c_str());
    }
    return bytes;
}


void WonderlandAdaptor::NetworkTask(char* URI, Wonderland::CachePolicy Policy, Wonderland::NetworkCallback _Callback){
    void* Buffer = nullptr;
    size_t bytes = this->GetHTMLSync(URI, Buffer, Policy);
    Wonderland::Status status ;
    if(bytes > 0)
        status = Wonderland::Status::SUCCESS;
    else
        status = Wonderland::Status::FAILED;
    delete URI;
    if(_Callback != nullptr){
        string strVar((char*)Buffer,bytes);
        _Callback(status,strVar);
    }
    if(Buffer != nullptr)
        free(Buffer);
    free(URI);
}


Wonderland::Status WonderlandAdaptor::NetworkFetch(const char* URI, std::string &Cookies, void *&Buffer, size_t &bytes){
    CURL *curl = curl_easy_init();          // new curl_session;
    if(curl){
        string data;
        curl_easy_setopt(curl, CURLOPT_URL, URI);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeToBuffer);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE,"");
        if(!Cookies.empty()){
            auto cookies_list = this->split(Cookies,"\n");
            for(auto item:cookies_list)
                if(item != "")
                    curl_easy_setopt(curl, CURLOPT_COOKIELIST, item.c_str());
        }
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
            struct curl_slist *cookies_updated;
            struct curl_slist *item;
            curl_easy_getinfo(curl, CURLINFO_COOKIELIST,&cookies_updated);
            item = cookies_updated;
            if(!item){
                DEBUG_MSG("[Cookies] No Cookies");
            }else{
                Cookies.clear();
                while(item){
                    Cookies.append(item->data);
                    Cookies.append("\n");
                    item = item->next;
                }
            }
            curl_slist_free_all(cookies_updated);
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
            std::string URI, \
            void *&Buffer,  \
            Wonderland::CachePolicy Policy){
    size_t bytes = this->GetHTMLSync(URI.c_str(), Buffer, Policy);
    if(bytes){
        string result((char*)Buffer);
        free(Buffer);
        result = this->ParseContent(URI, result);
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

void WonderlandAdaptor::GetParsedAsync(const char* URI,  \
        Wonderland::CachePolicy CachePolicy,     \
        Wonderland::NetworkCallback _Callback){
#if THREAD_POOL_SIZE != 0
    CHECK_PARAM_PTR(URI,)
    char* copyURI = (char*)malloc(strlen(URI)+1);
    memset(copyURI,0,strlen(URI)+1);
    strcpy(copyURI,URI);
    this->mThreadPool.AddTask(std::bind(&WonderlandAdaptor::ParseTask,this,copyURI,CachePolicy,_Callback));
#endif
    return;
}


void WonderlandAdaptor::ParseTask(const char* URI, Wonderland::CachePolicy Policy, Wonderland::NetworkCallback _Callback){
    void* Buffer = nullptr;
    size_t bytes = this->GetParsedSync(URI,Buffer, Policy);
    Wonderland::Status status ;
    if(bytes > 0)
        status = Wonderland::Status::SUCCESS;
    else
        status = Wonderland::Status::FAILED;
    delete URI;
    if(_Callback != nullptr){
        string strVar((char*)Buffer,bytes);
        _Callback(status,strVar);
    }
    if(Buffer != nullptr)
        free(Buffer);
    delete URI;
}

std::string WonderlandAdaptor::GetDomainFromURI(const char* URI){
    string domain(URI);
    domain = domain.substr(domain.find_first_of("::/")+3);
    domain = domain.substr(0, domain.find_first_of("/"));
    return domain;
}

std::vector<std::string> WonderlandAdaptor::split(std::string str, std::string pattern)
{
    std::string::size_type pos;
    std::vector<std::string> result;

    str += pattern;//扩展字符串以方便操作
    int size = str.size();

    for (int i = 0; i<size; i++) {
        pos = str.find(pattern, i);
        if (pos<size) {
            std::string s = str.substr(i, pos - i);
            result.push_back(s);
            i = pos + pattern.size() - 1;
        }
    }
    return result;
}