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
            string subject_id = "";
            string subject_name = "";
            string subject_type = "";
            {       // title
                CNode title = doc.find("h1.nameSingle").nodeAt(0) ;
                subject_name = title.find("a").nodeAt(0).text();          // get subject name
                j["title"] = subject_name;
                subject_id = title.find("a").nodeAt(0).attribute("href").substr(strlen("/subject/"));
                if(title.find("small").nodeNum() == 1)
                    j["subtype"] = title.find("small").nodeAt(0).text();    // ova , tv or others
                else
                    j["subtype"] = "";
                subject_type = doc.find("a.focus").nodeAt(0).attribute("href").substr(1);  // subject type, eg. real, game, anime...
                j["type"] = subject_type;
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
                    }else{
                        j["infobox"][key] = {{"text",info.substr(loc+2)}};     // skip ':' and space
                    }
                    j["infobox"][key]["links"] = link_j;
                    
                }
            }

            {       // recommend list 
                CSelection recommend_list = doc.find("#subjectPanelIndex").nodeAt(0).find("li.clearit");
                for(size_t i = 0; i < recommend_list.nodeNum(); ++i){
                    CNode item = recommend_list.nodeAt(i);
                    string avatar_header = item.find("span.avatarNeue").nodeAt(0).attribute("style");
                    avatar_header = avatar_header.substr(avatar_header.find_first_of("//"));
                    if(avatar_header.find_first_of('?') == string::npos){
                        avatar_header = avatar_header.substr(0,avatar_header.find_first_of('\''));
                    }else{
                        avatar_header = avatar_header.substr(0,avatar_header.find_first_of('?'));
                    }                    avatar_header[23] = 'l';        // change to size large
                    avatar_header = string(_BGM_PROTOCOL_).append(avatar_header);
                    
                    string avatar_link   = string(_BGM_PROTOCOL_).append("//").append(_BGM_DOMAIN_).append(item.find("span.avatarNeue").nodeAt(0).parent().attribute("href"));

                    CNode r_list_item = item.find("div.innerWithAvatar").nodeAt(0).find("a.avatar").nodeAt(0);
                    string list_link     = string(_BGM_PROTOCOL_).append("//").append(_BGM_DOMAIN_).append(r_list_item.attribute("href"));
                    string list_title    = r_list_item.text();

                    string avatar        = item.find("small.grey").nodeAt(0).find("a.avatar").nodeAt(0).text();

                    j["recommend_list"][list_title] = {
                        {"link",list_link},
                        {"avatar",{
                            {"name",avatar},
                            {"link",avatar_link},
                            {"header",avatar_header}
                        }}
                    };
                }
            }

            {       // Collection list
                CSelection collection_list = doc.find("#subjectPanelCollect").nodeAt(0).find("li.clearit");
                for(size_t i = 0; i < collection_list.nodeNum(); ++i){
                    CNode item = collection_list.nodeAt(i);
                    string avatar_header = item.find("span.avatarNeue").nodeAt(0).attribute("style");
                    avatar_header = avatar_header.substr(avatar_header.find_first_of("//"));
                    if(avatar_header.find_first_of('?') == string::npos){
                        avatar_header = avatar_header.substr(0,avatar_header.find_first_of('\''));
                    }else{
                        avatar_header = avatar_header.substr(0,avatar_header.find_first_of('?'));
                    }
                    avatar_header[23] = 'l';        // change to size large
                    avatar_header = string(_BGM_PROTOCOL_).append(avatar_header);
                    
                    string avatar_link   = string(_BGM_PROTOCOL_).append("//").append(_BGM_DOMAIN_).append(item.find("span.avatarNeue").nodeAt(0).parent().attribute("href"));

                    CNode avatar_node = item.find("div.innerWithAvatar").nodeAt(0);
                    string avatar = avatar_node.find("a.avatar").nodeAt(0).text();

                    string collection_status = avatar_node.find("small.grey").nodeAt(0).text();

                    j["collection_list"][avatar] = {
                        {"link", avatar_link},
                        {"header", avatar_header},
                        {"collection_status", collection_status}
                    };

                    if(avatar_node.find("span.starsinfo").nodeNum() != 0){
                        string star_info = avatar_node.find("span.starsinfo").nodeAt(0).attribute("class");
                        star_info = star_info.substr(0, star_info.find_first_of(' '));
                        int stars = atoi(star_info.substr(strlen("sstars")).c_str());
                        j["collection_list"][avatar]["stars"] = stars;
                    }
                }
            }

            {       // collection status
                CSelection collection_status = doc.find("#subjectPanelCollect").nodeAt(0).find("span.tip_i").nodeAt(0).find("a.l");
                for(size_t i = 0; i < collection_status.nodeNum(); ++i){
                    CNode item = collection_status.nodeAt(i);
                    string txt = item.text();
                    size_t loc = txt.find_first_of("人");
                    int num = atoi(txt.substr(0,loc).c_str());
                    string status = txt.substr(loc+strlen("人"));
                    j["collection_status"][status] = num;
                }
            }

            if(doc.find("ul.prg_list").nodeNum() != 0){     // prg_list // TODOs: EP STATUS!
                CSelection ep_list = doc.find("ul.prg_list").nodeAt(0).find("li");
                CNode      ep_info = doc.find("#subject_prg_content").nodeAt(0);
                string Section = "本篇";
                for(size_t i = 0; i < ep_list.nodeNum(); ++i){
                    CNode item = ep_list.nodeAt(i);
                    string clas = item.attribute("class");
                    if(clas == "subtitle"){
                        Section = item.text();
                    }else{
                        item = item.find("a").nodeAt(0);
                        string ep_index = item.ownText();
                        j["ep_list"][Section][ep_index]["air"] = item.attribute("class").substr(strlen("load-epinfo epBtn"));

                        j["ep_list"][Section][ep_index]["title"]  = item.attribute("title");
                        j["ep_list"][Section][ep_index]["uri"]    = string(_BGM_PROTOCOL_).append(_BGM_DOMAIN_).append(item.attribute("href"));
                        j["ep_list"][Section][ep_index]["id"]     = item.attribute("id").substr(strlen("prg_"));

                        string infobox = ep_info.find(string("#prginfo_").append(item.attribute("id").substr(strlen("prg_")))).nodeAt(0).childAt(0).text();
                        CNode debug_node = ep_info.find(string("#prginfo_").append(item.attribute("id").substr(strlen("prg_")))).nodeAt(0).childAt(0);
                        CNode child_item;
                        for(int k = 0; k < debug_node.childNum(); ++k){
                            child_item = debug_node.childAt(k);
                            if(child_item.tag() == "span"){   // comments num
                                string str = child_item.text();
                                int num_start = str.find_first_of('+')+1;
                                int num_end = str.find_first_of(')');
                                j["ep_list"][Section][ep_index]["comments_num"] = str.substr(num_start,num_end-num_start);
                            }else if(child_item.tag() == "br" || child_item.tag() == "hr"){   // continue
                                continue;
                            }else{          // other info
                                string key;
                                string value;
                                string str = child_item.text();
                                int sp_index = str.find_first_of(':');
                                key = str.substr(0,sp_index);
                                value = str.substr(sp_index+1,str.length());
                                j["ep_list"][Section][ep_index]["info"][key] = value;
                            }
                        }
                    }
                }
            }
        }else if(t_str == "user"){  // user page
            return "";
        }else{
            return "";
        }
    }
    return j.dump();
}
