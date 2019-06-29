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

bool BangumiAdaptor::CheckURI(const string &URI) const {
    auto splited = split(URI.substr(URI.find_first_of("://")+3), "/");
    if(splited.size() == 0){
        DEBUG_MSG("Wrong URI param [" << URI << "]");
        return false;
    }
    string domain = splited[0];
    if(domain == "bgm.tv" or domain == "bangumi.tv")
        return true;
    return false;
}

std::string BangumiAdaptor::ParseContent(string URI, const string &Data ) const{
    if(Data.empty())    return "";
    URI = URI.substr(URI.find_first_of("://")+3);
    std::vector<std::string> splited = split(URI,"/");
    
    if(splited.size() == 0){
        return "";
    }

    json j;
    std::string return_str = "";
    CDocument doc;
    doc.parse(Data);
    if(splited.size() == 1){        // homepage
        return "";
    }else{
        string t_str = splited[1];
        if(t_str == "subject"){     // subject page

            {       // title
                CNode title = doc.find("h1.nameSingle").nodeAt(0) ;
                j["title"] = title.find("a").nodeAt(0).text();          // get subject name
                if(title.find("small").nodeNum() == 1)
                    j["subtype"] = title.find("small").nodeAt(0).text();    // ova , tv or others
                else
                    j["subtype"] = "";
                j["type"] = doc.find("a.focus").nodeAt(0).attribute("href").substr(1);  // subject type, eg. real, game, anime...
            }
            
            {       // image 
                string imageURI_medium = string(_BGM_PROTOCOL_).append(doc.find("img.cover").nodeAt(0).attribute("data-cfsrc"));
                CNode img = doc.find("img.cover").nodeAt(0);
                string imageURI_small = imageURI_medium;
                imageURI_small[30] = 's';
                string imageURI_large = imageURI_medium;
                imageURI_large[30] = 'l';
                j["cover"] = {
                    {"medium",imageURI_medium},
                    {"large",imageURI_large},
                    {"small",imageURI_small}
                };
            }

            {       // info box
                CSelection infobox = doc.find("#infobox").nodeAt(0).find("li");
                for (size_t i = 0; i < infobox.nodeNum(); ++i){
                    CNode item = infobox.nodeAt(i);
                    string info = item.text();
                    int loc = info.find_first_of(':');
                    std::string key = info.substr(0,loc);
                    
                    CSelection links = item.find("a");
                    json link_j = json::object();
                    for(size_t j = 0; j < links.nodeNum(); ++j){
                        CNode links_item = links.nodeAt(j);    
                        link_j[links_item.ownText()] = string(_BGM_PROTOCOL_).append("//").append(_BGM_DOMAIN_).append(links_item.attribute("href"));
                    }
                    if(j["infobox"][key] != nullptr){
                        j["infobox"][key]["text"] =  string(j["infobox"][key]["text"]).append("\n").append(info.substr(loc+2));
                        link_j.merge_patch(j["infobox"][key]["links"]);
                        j["infobox"][key]["links"] = link_j;
                    }else{
                        j["infobox"][key] = {
                            {"text",info.substr(loc+2)},     // skip ':' and space
                            {"links",link_j}
                        };
                    }
                    
                }
            }

            {       // recommend list 

            }

            {       // playing list

            }

        }else if(t_str == "user"){  // user p

        }else if(t_str == "user"){  // user page
            return "";
        }else{
            return "";
        }
    }
    return j.dump();
}