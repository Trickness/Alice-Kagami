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
    if(domain == "bgm.tv" or domain == "bangumi.tv"){
        if(URI.substr(0,strlen("https")) != "https"){
            DEBUG_MSG("Please https instead of http!");
            return false;
          }
          return true;
    }
    return false;
}

// TODOs:
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
				CNode tmpNode = title.find("a").nodeAt(0);
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

            if(doc.find("img.cover").nodeNum() != 0){       // image
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
                        j["infobox"][key]["text"] =  j["infobox"][key]["text"].dump().append("\n").append(info.substr(loc+2));
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
                    avatar_header = BangumiAdaptor::ParseImageURI(avatar_header,strlen("user"));

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
                    avatar_header = BangumiAdaptor::ParseImageURI(avatar_header,strlen("user"));

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
                    //size_t loc = txt.find("人");
					size_t loc = txt.find("人\0");
                    int num = atoi(txt.substr(0,loc).c_str());
                    string status = txt.substr(loc+strlen("人\0"));
                    j["collection_status"][status] = num;
                }
            }

            if(doc.find("ul.prg_list").nodeNum() != 0){     // prg_list
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

            {   // subject summary
                CSelection summary = doc.find("#subject_summary");
                if(summary.nodeNum() == 1){
                    j["summary"] = summary.nodeAt(0).text();
                }
            }

            if(doc.find("div.subject_tag_section").nodeNum() != 0){   // tags
                CSelection tags = doc.find("div.subject_tag_section").nodeAt(0).find("a");
                for(size_t i = 0; i < tags.nodeNum(); ++i){
                    CNode item = tags.nodeAt(i);
                    j["tags"][item.find("span").nodeAt(0).ownText()] = item.find("small").nodeAt(0).ownText();
                }
            }

            {   // rating
                CNode rating_box = doc.find("div.SidePanel").nodeAt(0);
                j["rating"]["score"] = rating_box.find("span.number").nodeAt(0).ownText();
                j["rating"]["description"] = rating_box.find("span.description").nodeAt(0).ownText();
                if(rating_box.find("small.alarm").nodeNum() == 1){
                    j["rating"]["rank"] = rating_box.find("small.alarm").nodeAt(0).ownText().substr(1);
                }else{
                    j["rating"]["rank"] = "--";
                }

                CSelection detail = rating_box.find("li");
                for(size_t k = 0; k < 10; ++k){
                    CNode detail_item = detail.nodeAt(k).childAt(0);
                    string count = detail_item.find("span.count").nodeAt(0).ownText();
                    j["rating"]["distribution"][detail_item.find("span.label").nodeAt(0).ownText()] = count.substr(1,count.length()-2);
                }
            }

            if(doc.find("#browserItemList").nodeNum() != 0){    // characters
                CSelection chara_list = doc.find("#browserItemList").nodeAt(0).find("li");
                for(size_t i = 0; i < chara_list.nodeNum(); ++i){
                    CNode chara_item = chara_list.nodeAt(i);
                    CNode t_node = chara_item.find("strong").find("a").nodeAt(0);
                    string chara_name = t_node.ownText();
                    j["characters"][chara_name]["id"] = t_node.attribute("href").substr(strlen("/character/"));

                    string str = t_node.attribute("title");
                    if(str.find_first_of(" / ") != string::npos){
                        j["characters"][chara_name]["translation"] = str.substr(str.find_first_of('/')+strlen("/ "));
                    }else{
                        j["characters"][chara_name]["translation"] = "";
                    }

                    string count = chara_item.find("small.fade").nodeAt(0).ownText();
                    j["characters"][chara_name]["comments_num"] = count.substr(2,count.length()-3);

                    string avatar_header = t_node.find("span.avatarNeue").nodeAt(0).attribute("style");
                    j["characters"][chara_name]["header"] = BangumiAdaptor::ParseImageURI(avatar_header,strlen("crt"));

                    t_node = chara_item.find("span.tip_j").nodeAt(0);
                    j["characters"][chara_name]["job"] = t_node.find("span.badge_job_tip").nodeAt(0).ownText();

                    CSelection cvs = t_node.find("a");
                    for(size_t k = 0; k < cvs.nodeNum(); ++k){
                        CNode cv = cvs.nodeAt(k);
                        j["characters"][chara_name]["cv"][cv.ownText()] = string(_BGM_PROTOCOL_).append(_BGM_DOMAIN_).append(cv.attribute("href"));
                    }
                }
            }
            if(doc.find("ul.browserCoverSmall").nodeNum() != 0){   // subdivision (单行本之类)
                CNode top = doc.find("ul.browserCoverSmall").nodeAt(0).parent();
                j["subdivision"]["title"] = top.find("h2.subtitle").nodeAt(0).ownText();
                CSelection subdivisions = top.find("a");

                for(size_t i = 0; i < subdivisions.nodeNum(); ++i){
                    CNode item = subdivisions.nodeAt(i);
                    string title = item.attribute("title");
                    j["subdivision"]["list"][title]["id"] = item.attribute("href").substr(strlen("/subject/"));
                    j["subdivision"]["list"][title]["cover"] = BangumiAdaptor::ParseImageURI(item.childAt(0).attribute("style"),strlen("cover"));
                }
            }
            if(doc.find("ul.browserCoverMedium").nodeNum() != 0){   // related subjects
                CNode top = doc.find("ul.browserCoverMedium").nodeAt(0);
                string tag = "untagged";
                CSelection subdivisions = top.find("li");
                for(size_t i = 0; i < subdivisions.nodeNum(); ++i){
                    CNode item = subdivisions.nodeAt(i);
                    if(item.attribute("class") == "sep"){
                        tag = item.find("span.sub").nodeAt(0).ownText();
                    }
                    j["related"][tag]["id"] = item.find("a.title").nodeAt(0).attribute("href").substr(strlen("/subject/"));
                    j["related"][tag]["cover"] = BangumiAdaptor::ParseImageURI(item.find("span.avatarNeue").nodeAt(0).attribute("style"),strlen("cover"));
                    j["related"][tag]["title"] = item.find("a.title").nodeAt(0).ownText();
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
