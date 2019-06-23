#include "include/Bangumi/BangumiAdaptor.hpp"
#include "src/third-party/gumbo-parser/Document.h"
#include "src/third-party/gumbo-parser/Node.h"
#include "src/third-party/nlohmann/json.hpp"
#include <vector>

using namespace std;
using json = nlohmann::json;



BangumiAdaptor::BangumiAdaptor()
    :WonderlandAdaptor(){
            DEBUG_MSG("Bangumi Adaptor Version 0.1 loaded.");
}

BangumiAdaptor::~BangumiAdaptor(){

}

bool BangumiAdaptor::CheckURI(const char* URI) const {
    CHECK_PARAM_PTR(URI,false);
    string uri(URI);
    uri = uri.substr(uri.find_first_of("://")+3);
    auto splited = split(uri, "/");
    if(splited.size() == 0){
        DEBUG_MSG("Wrong URI param [" << URI << "]");
        return false;
    }
    string domain = splited[0];
    if(domain == "bgm.tv" or domain == "bangumi.tv")
        return true;
    return false;
}

std::string BangumiAdaptor::ParseContent(const char* URI, const void* Data, size_t Bytes) const{
    if(Bytes == 0)
        return "";
    CHECK_PARAM_PTR(URI,"");
    CHECK_PARAM_PTR(Data,"");
    std::string uri(URI);
    std::string html((char*)Data);
    uri = uri.substr(uri.find_first_of("://")+3);
    std::vector<std::string> splited = split(uri,"/");
    
    if(splited.size() == 0){
        return "";
    }

    json j;
    std::string return_str = "";
    CDocument doc;
    doc.parse(html);
    if(splited.size() == 1){        // homepage

    }else{
        string t_str = splited[1];
        if(t_str == "subject"){     // subject page
            CNode node = doc.find("h1.nameSingle").nodeAt(0).find("a").nodeAt(0);
            j["title"] = node.text();
        }else if(t_str == "user"){  // user page
        }
    }
    return j.dump();
}