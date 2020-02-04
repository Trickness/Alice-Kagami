#ifndef _WONDERLAND_ADAPTOR_
#define _WONDERLAND_ADAPTOR_

#include <iostream>
#include <functional>
#include <curl/curl.h>
#include <csetjmp> // for seg fault control (setjmp)
#include <csignal> // for set fault control (signal)
#include "Config.hpp"
#include "CastBook.hpp"
#include "ThreadPool.hpp"

#ifdef _MSC_VER
#include <windows.h>
#endif

/*  Network Connector & Cache controller */
/*  Should only one instance in the program */

namespace Wonderland{
    enum class CachePolicy:unsigned int{ONLY_FROM_CACHE, FIRST_FROM_CACHE, NEVER_FROM_CACHE};    
    enum class Status:unsigned int{SUCCESS, FAILED};
    //      return_status              data     
    typedef std::function<void(Wonderland::Status, std::string)> NetworkCallback; 
}

class WonderlandAdaptor{
    public:
        WonderlandAdaptor(std::string name, std::string version, std::string author);
        ~WonderlandAdaptor(); 
        
        void GetHTMLAsync( 
            const std::string URI, 
            Wonderland::CachePolicy Policy = Wonderland::CachePolicy::FIRST_FROM_CACHE,    
            Wonderland::NetworkCallback _Callback = nullptr   
        );

        /* Buffer need to be a void ptr, and you should free() it yourself (thread safe)*/
        size_t GetHTMLSync( 
            const  std::string URI, 
            void *&Buffer,  
            Wonderland::CachePolicy Policy = Wonderland::CachePolicy::FIRST_FROM_CACHE      
        );

        void GetParsedAsync(  
            std::string URI, 
            Wonderland::CachePolicy Policy = Wonderland::CachePolicy::FIRST_FROM_CACHE,    
            Wonderland::NetworkCallback _Callback = nullptr   
        );

        /* Buffer need to be a void ptr, and you should free() it yourself, (thread safe) */
        size_t GetParsedSync( 
            std::string URI, 
            void *&Buffer,  
            Wonderland::CachePolicy Policy = Wonderland::CachePolicy::FIRST_FROM_CACHE      
        );

        bool CheckLoginStatusSync() const{
            return false;
        };

        void CheckLoginStatusAsync(Wonderland::NetworkCallback _Callback){
            this->mThreadPool.AddTask(std::bind([](WonderlandAdaptor *&parent, Wonderland::NetworkCallback __Callback){
                if(parent->CheckLoginStatusSync()){__Callback(Wonderland::Status::SUCCESS, std::string("{\"login_status\": true }"));
                }else{__Callback(Wonderland::Status::FAILED, std::string("{\"login_status\": false }"));}},this, _Callback));}

        void   CacheResource(const char* URI, const void *Buffer, size_t Bytes);

        static std::string GetDomainFromURI(const char* URI);
        static std::vector<std::string> split(std::string str, std::string pattern);


        virtual bool CheckURI(const std::string &URI) const = 0;
        std::string GetModuleName(void){return ModuleName;};
        std::string GetModuleVersion(void){return ModuleVersion;};
        std::string GetModuleAuthor(void){return ModuleAuthor;};

        virtual void SegFaultHandler_Parse(void){};

    protected:
        WonderlandAdaptor(const WonderlandAdaptor &);
        void NetworkTask(std::string URI, Wonderland::CachePolicy, Wonderland::NetworkCallback _Callback);
        Wonderland::Status NetworkFetch(const char* URI,std::string &, void *&Buffer, size_t &bytes);
        void ParseTask(const char*,Wonderland::CachePolicy, Wonderland::NetworkCallback _Callback);

        virtual std::string ParseContent(std::string ,const std::string &) const = 0;

        std::string ModuleName;
        std::string ModuleVersion;
        std::string ModuleAuthor;

    private:
        CastBook *mCastBook;
        ThreadPool mThreadPool;
}; 

#endif