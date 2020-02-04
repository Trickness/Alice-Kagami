#include "include/WonderlandAdaptor.hpp"
#include <string>
#include <vector>
#include <curl/curl.h>

using namespace std;

#ifdef __linux__
pthread_key_t env_key;
pthread_key_t self_key;
#elif _MSC_VER
DWORD env_key;
DWORD self_key;
#endif

size_t writeToBuffer(void *ptr, size_t size, size_t count, void *buffer){
    ((string*)buffer)->append((char*)ptr,0,size * count);
    return size * count;
}

WonderlandAdaptor::WonderlandAdaptor(string name, string version, string author){
    this->mCastBook = new CastBook();
    this->ModuleName = name;
    this->ModuleVersion = version;
    this->ModuleAuthor = author;
    DEBUG_MSG(this->ModuleName << " version " << this->ModuleVersion << " is loading...");
}

WonderlandAdaptor::~WonderlandAdaptor(){
    delete this->mCastBook;
}


void WonderlandAdaptor::GetHTMLAsync(const std::string URI,  \
        Wonderland::CachePolicy CachePolicy,     \
        Wonderland::NetworkCallback _Callback){

    CHECK_PARAM_STR(URI,)
#if THREAD_POOL_SIZE != 0
    this->mThreadPool.AddTask(std::bind(&WonderlandAdaptor::NetworkTask,this,URI,CachePolicy, _Callback));
#else
    void *Buffer = nullptr;
    auto bytes = this->GetHTMLSync(URI, Buffer, CachePolicy);
    if(_Callback){
        if(bytes){
            string strVar((char*)Buffer,bytes);
            _Callback(Wonderland::Status::SUCCESS,strVar);
        }else{
            _Callback(Wonderland::Status::FAILED,"");
        }
    }
    if(Buffer)  free(Buffer);
#endif
    return;
}


size_t WonderlandAdaptor::GetHTMLSync(const std::string URI,  \
        void *&Buffer,      \
        Wonderland::CachePolicy CachePolicy){
    CHECK_PARAM_STR(URI,0)
    CastBook *castBook = new CastBook();
    if(CachePolicy == Wonderland::CachePolicy::FIRST_FROM_CACHE || CachePolicy == Wonderland::CachePolicy::ONLY_FROM_CACHE){
        unique_ptr<CastBookRecord> result = castBook->GetRecord(URI.c_str());
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
    string Domain = this->GetDomainFromURI(URI.c_str());
    string Cookies = castBook->GetRosterCookies(Domain.c_str());

    Wonderland::Status status = this->NetworkFetch(URI.c_str(),Cookies, Buffer, bytes);
    if(status == Wonderland::Status::SUCCESS){
        castBook->PutRecord(URI.c_str(), Buffer, bytes);
        castBook->SetRosterCookies(Domain.c_str(), Cookies.c_str());
    }
    return bytes;
}


void WonderlandAdaptor::NetworkTask(std::string URI, Wonderland::CachePolicy Policy, Wonderland::NetworkCallback _Callback){
    void* Buffer = nullptr;
    size_t bytes = this->GetHTMLSync(URI, Buffer, Policy);
    Wonderland::Status status ;
    if(bytes > 0)
        status = Wonderland::Status::SUCCESS;
    else
        status = Wonderland::Status::FAILED;
    if(_Callback != nullptr){
        string strVar((char*)Buffer,bytes);
        _Callback(status,strVar);
    }
    if(Buffer != nullptr)
        free(Buffer);
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
            if (!Buffer) {
                DEBUG_MSG("Failed to alloc memory!");
                ExitThread(0);
            }
            memset(Buffer,0,bytes+1);
            strcpy((char*)Buffer,curl_easy_strerror(res));
        }else{
            bytes = data.length();
            Buffer= malloc(bytes+1);
            if (!Buffer) {
                DEBUG_MSG("Failed to alloc memory!");
                ExitThread(0);
            }
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

void DefaultSegFaultHandler_Parse(int signum){
#ifdef __linux__
    WonderlandAdaptor* self = (WonderlandAdaptor*) pthread_getspecific(self_key);
    jmp_buf* env = (jmp_buf*)pthread_getspecific(env_key);
#elif _MSC_VER
    WonderlandAdaptor* self = (WonderlandAdaptor*)TlsGetValue(self_key);
    jmp_buf* env = (jmp_buf*)TlsGetValue(env_key);
#endif
    printf("\n");
    DEBUG_MSG(" ---- Segfault encountered during parsing! ----");
    DEBUG_MSG("       Module Name : " << self->GetModuleName());
    DEBUG_MSG("    Module Version : " << self->GetModuleVersion());
    DEBUG_MSG("     Module Author : " << self->GetModuleAuthor());
    DEBUG_MSG(" ----                 END                  ----");
    printf("\n");
    
    self->SegFaultHandler_Parse();      // 调用自定义处理函数

    signal(SIGSEGV,SIG_DFL);
    longjmp(*env,1);
}

size_t WonderlandAdaptor::GetParsedSync( \
            std::string URI, \
            void *&Buffer,  \
            Wonderland::CachePolicy Policy){
    size_t bytes = this->GetHTMLSync(URI.c_str(), Buffer, Policy);
    if (bytes) {
        string result((char*)Buffer);
        free(Buffer);
#ifdef __linux__
        jmp_buf* env = (jmp_buf*)malloc(sizeof(jmp_buf));
        pthread_key_create(&env_key, NULL);
        pthread_key_create(&self_key, NULL);
        pthread_setspecific(env_key, (void*)env);
        pthread_setspecific(self_key, (void*)this);
#elif _MSC_VER
        jmp_buf* env = (jmp_buf*)LocalAlloc(LPTR, sizeof(jmp_buf));
        if (env == nullptr) {
            DEBUG_MSG("Failed to alloc local memory!");
            ExitThread(1);
        }
        if ((env_key = TlsAlloc()) == TLS_OUT_OF_INDEXES) {
            DEBUG_MSG("Failed to alloc Thread Local Storage for JMP_ENV");
            ExitThread(1);
        }
        if ((self_key = TlsAlloc()) == TLS_OUT_OF_INDEXES) {
            DEBUG_MSG("Failed to alloc Thread Local Storage for THIS");
            ExitThread(1);
        }
        TlsSetValue(env_key, (LPVOID)env);
        TlsSetValue(self_key, (LPVOID)this);
#endif
        int r = setjmp(*env);
        if (r == 0) {
            signal(SIGSEGV, DefaultSegFaultHandler_Parse);
            result = this->ParseContent(URI, result);
            signal(SIGSEGV, SIG_DFL);
        }
        else {
#ifdef __linux
            jmp_buf* env = (jmp_buf*)pthread_getspecific(env_key);
            free(env);
            pthread_key_delete(env_key);
            pthread_key_delete(self_key);
            return 0;
        }
        jmp_buf* env = (jmp_buf*)pthread_getspecific(env_key);
        free(env);
        pthread_key_delete(env_key);
        pthread_key_delete(self_key);
#elif _MSC_VER
            jmp_buf* env = (jmp_buf*)TlsGetValue(env_key);
            LocalFree(env);
            TlsFree(env_key);
            TlsFree(self_key);
            return 0;
        }
        env = (jmp_buf*)TlsGetValue(env_key);
        LocalFree(env);
        TlsFree(env_key);
        TlsFree(self_key);
#endif
        if(result.length() != 0){
            bytes = result.length();
            Buffer = (char*) malloc(bytes+1);
            if (!Buffer) {
                DEBUG_MSG("Failed to alloc memory!");
                ExitThread(0);
            }
            memset(Buffer,0,bytes+1);
            memcpy(Buffer, result.c_str(), bytes);
        }else{
            bytes = 0;
        }
    }
    return bytes;
}

void WonderlandAdaptor::GetParsedAsync(         \
        std::string URI,                        \
        Wonderland::CachePolicy CachePolicy,     \
        Wonderland::NetworkCallback _Callback){
    CHECK_PARAM_STR(URI,_Callback(Wonderland::Status::FAILED,""))
#if THREAD_POOL_SIZE != 0
    this->mThreadPool.AddTask(std::bind(&WonderlandAdaptor::ParseTask,this,URI.c_str(),CachePolicy,_Callback));
#else
    this->ParseTask(URI.c_str(),CachePolicy,_Callback);
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

    str += pattern;
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
