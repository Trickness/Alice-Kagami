#include "include/Bangumi/BangumiAdaptor.hpp"
#include "src/third-party/gumbo-parser/Document.h"
#include "src/third-party/gumbo-parser/Node.h"
#include <vector>

using namespace std;
using json = nlohmann::json;

BangumiAdaptor::BangumiAdaptor()
    :WonderlandAdaptor("Bangumi","0.01","sternwzhang(sternwzhang@outlook.com)"){
}

BangumiAdaptor::~BangumiAdaptor(){

}

void BangumiAdaptor::SegFaultHandler_Parse(void){
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

json BangumiAdaptor::ParseUserHomepagePictureList(CNode d, std::string separator){
    json v = json::object();
    if(d.find("div.horizontalOptions").nodeNum() == 1){
        
    }
    if(d.find("div.clearit").nodeNum() != 0){
        CSelection ls = d.find("div.clearit");
        for(auto index = 0; index < ls.nodeNum(); ++index){
            CNode tmpItem = ls.nodeAt(index);
            if(tmpItem.attribute("class") != "clearit"){
                CSelection ls = tmpItem.find("li");
                v["overview"] = {};
                std::string title = "";
                for(auto index = 0; index < ls.nodeNum(); ++index){
                    CNode item = ls.nodeAt(index);
                    if(item.attribute("class") == "title"){
                        title = item.text();
                    }else{
                        std::string txt = item.text();
                        std::vector<std::string> list = split(txt,separator);
                        v["overview"][list[1]] = stoi(list[0]);
                    }
                }
                v["title"] = title;
            }else{
                string status = tmpItem.find("div.substatus").nodeAt(0).ownText();
                CSelection lss = tmpItem.find("li");
                std::vector<json> vec;
                for(auto i = 0; i < lss.nodeNum(); ++i){
                    CNode item = lss.nodeAt(i);
                    json j = json::object();
                    CNode a = item.find("a").nodeAt(0);
                    j["title"] = a.attribute("title");
                    j["link"] = string(_BGM_PROTOCOL_) + "//" + _BGM_DOMAIN_ + a.attribute("href");
                    j["id"] = stoi(a.attribute("href").substr(strlen("/subject/")));
                    j["image"] = BangumiAdaptor::ParseImageURI(a.find("img").nodeAt(0).attribute("data-cfsrc"),strlen("cover"));
                    vec.push_back(j);
                }
                v[status] = vec;
            }
            
        }
    }
    return v;
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
            if (doc.find("h1.nameSingle").nodeNum() != 0){  // title
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
            }else{
                j["message"] = doc.find("div.message").nodeAt(0).find("p.text").nodeAt(0).text();
                j["URI"] = URI;
                string json_str = "";
                try {
                    json_str = j.dump();
                }
                catch (char *e) {
                    DEBUG_MSG(e);
                    free(e);
                }
                return json_str;
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

            if(doc.find("#infobox").nodeNum() != 0){       // info box
                CSelection infobox = doc.find("#infobox").nodeAt(0).find("li");
		        j["infobox"] = json::array();
                for (size_t i = 0; i < infobox.nodeNum(); ++i){
                    CNode item = infobox.nodeAt(i);
                    string info = item.text();
                    int loc = info.find_first_of(':');
                    std::string key = info.substr(0,loc);
                    CSelection links = item.find("a");
                    json link_j = json::object();
		            json v = json::object();
                    for(size_t j = 0; j < links.nodeNum(); ++j){
                        CNode links_item = links.nodeAt(j);
                        link_j[links_item.ownText()] = string(_BGM_PROTOCOL_).append("//").append(_BGM_DOMAIN_).append(links_item.attribute("href"));
                    }
                    if(v[key] != nullptr){
                        string s = v[key]["text"];
                        v[key]["text"] = s.append("\n").append(info.substr(loc+2));
                        //v[key]["text"] =  string(v[key]["text"]).append("\n").append(info.substr(loc+2));
                        link_j.merge_patch(v[key]["links"]);
                    }else{
                        v[key] = {{"text",info.substr(loc+2)}};     // skip ':' and space
                    }
                    v[key]["links"] = link_j;
		            j["infobox"].push_back(v);
                }
            }
            if(doc.find("#subjectPanelIndex").nodeNum() != 0){       // related index
                CSelection related_index = doc.find("#subjectPanelIndex").nodeAt(0).find("li.clearit");
                for(size_t i = 0; i < related_index.nodeNum(); ++i){
                    CNode item = related_index.nodeAt(i);
                    string avatar_header = item.find("span.avatarNeue").nodeAt(0).attribute("style");
                    avatar_header = BangumiAdaptor::ParseImageURI(avatar_header,strlen("user"));

                    string avatar_link   = string(_BGM_PROTOCOL_).append("//").append(_BGM_DOMAIN_).append(item.find("span.avatarNeue").nodeAt(0).parent().attribute("href"));

                    CNode r_list_item = item.find("div.innerWithAvatar").nodeAt(0).find("a.avatar").nodeAt(0);
                    string list_link     = string(_BGM_PROTOCOL_).append("//").append(_BGM_DOMAIN_).append(r_list_item.attribute("href"));
                    string list_title    = r_list_item.text();

                    string avatar        = item.find("small.grey").nodeAt(0).find("a.avatar").nodeAt(0).text();

                    j["related_index"][list_title] = {
                        {"link",list_link},
                        {"avatar",{
                            {"name",avatar},
                            {"link",avatar_link},
                            {"header",avatar_header}
                        }}
                    };
                }
            }

            if(doc.find("#subjectPanelCollect").nodeNum() != 0){       // Collection list
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

            if(doc.find("#subjectPanelCollect").nodeNum() != 0){       // collection status
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
                                j["ep_list"][Section][ep_index]["comments"] = str.substr(num_start,num_end-num_start);
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
                    j["characters"][chara_name]["comments"] = count.substr(2,count.length()-3);

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
            if(doc.find("ul.browserCoverSmall").nodeNum() != 0){   // subdivision 
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
                    string title = item.find("a.title").nodeAt(0).ownText();;
                    j["related"][tag][title]["id"] = item.find("a.title").nodeAt(0).attribute("href").substr(strlen("/subject/"));
                    j["related"][tag][title]["cover"] = BangumiAdaptor::ParseImageURI(item.find("span.avatarNeue").nodeAt(0).attribute("style"),strlen("cover"));
                    j["related"][tag][title]["translation"] = item.find("a").nodeAt(0).attribute("title"); // ugly fix
                }
            }

            if(doc.find("ul.coversSmall").nodeNum() != 0){   // recommend subjects
                CSelection recommends = doc.find("ul.coversSmall").nodeAt(0).find("li");
                for(size_t i = 0; i < recommends.nodeNum(); ++i){
                    CNode item = recommends.nodeAt(i);
                    string title = item.find("a.l").nodeAt(0).ownText();
                    j["recommend"][title]["id"] = item.find("a.l").nodeAt(0).attribute("href").substr(strlen("/subject/"));
                    j["recommend"][title]["translation"] = item.find("a.avatar").nodeAt(0).attribute("title");
                    j["recommend"][title]["cover"] = BangumiAdaptor::ParseImageURI(item.find("span.avatarNeue").nodeAt(0).attribute("style"),strlen("cover"));
                    
                }
            }

            if(doc.find("#entry_list").nodeNum() != 0){
                CSelection comments = doc.find("#entry_list").nodeAt(0).find("div.item");
                for(size_t i = 0; i < comments.nodeNum(); ++i){
                    CNode item = comments.nodeAt(i);
                    CNode blog = item.find("div.entry").nodeAt(0);
                    CNode title_node = blog.find("h2.title").nodeAt(0).find("a").nodeAt(0);
                    CNode header_node = item.find("p.cover").nodeAt(0);
                    CNode user_node = blog.find("span.tip_j").nodeAt(0).find("a").nodeAt(0);
                    string title = title_node.ownText();
                    j["blogs"][title]["id"] = title_node.attribute("href").substr(strlen("/blog/"));
                    j["blogs"][title]["user"]["header"] = BangumiAdaptor::ParseImageURI(header_node.find("img").nodeAt(0).attribute("data-cfsrc"),strlen("user"));
                    j["blogs"][title]["user"]["id"] = user_node.attribute("href").substr(strlen("/user/"));
                    j["blogs"][title]["user"]["avatar"] = user_node.ownText();
                    j["blogs"][title]["time"] = item.find("small.time").nodeAt(0).ownText();
                    string comment_str = item.find("small.orange").nodeAt(0).ownText();
                    j["blogs"][title]["replies"] = comment_str.substr(strlen("(+"),comment_str.length() - strlen("+()"));
                    j["blogs"][title]["preview"] = blog.find("div.content").nodeAt(0).ownText();
                }
            }

            if(doc.find("table.topic_list").nodeNum() != 0){
                CSelection topics = doc.find("table.topic_list").nodeAt(0).find("tr");
                for(size_t i = 0; i < topics.nodeNum(); ++i){
                    CNode item = topics.nodeAt(i);
                    if(item.find("a.l").nodeNum() == 0)     break;      // ugly hack
                    CNode title_node = item.find("a.l").nodeAt(0);
                    CSelection cs = item.find("td");
                    CNode user_node  = cs.nodeAt(1).find("a").nodeAt(0);
                    string title = title_node.attribute("title");
                    j["topic"][title]["id"] = title_node.attribute("href").substr(strlen("/subject/topic/"));
                    j["topic"][title]["user"]["id"] = user_node.attribute("href").substr(strlen("/user/"));
                    j["topic"][title]["user"]["avatar"] = user_node.ownText();
                    string replies = cs.nodeAt(2).text();
                    j["topic"][title]["replies"] = replies.substr(0,replies.find_first_of(" "));
                    j["topic"][title]["time"] = cs.nodeAt(3).text();
                }
            }
            {       // comments
                CSelection comments = doc.find("#comment_box").find("div.item");
                json v = json::array();
                for(size_t i = 0; i < comments.nodeNum(); ++i){
                    CNode item = comments.nodeAt(i);
                    CNode user_node = item.find("a.l").nodeAt(0);
                    string avatar_id = user_node.attribute("href").substr(strlen("/user/"));
                    json t = json::object();
                    t["avatar"] = {
                        {"id",avatar_id},
                        {"name",user_node.ownText()},
                        {"header",BangumiAdaptor::ParseImageURI(item.find("span.avatarNeue").nodeAt(0).attribute("style"),strlen("user"))}
                    };
                    t["time"] = item.find("small.grey").nodeAt(0).ownText().substr(strlen("@ "));
                    t["text"] = item.find("p").nodeAt(0).ownText();
                    if(item.find("span.starlight").nodeNum() != 0){
                        string star_info = item.find("span.starlight").nodeAt(0).attribute("class");
                        t["stars"] = star_info.substr(strlen("starlight stars"));
                    }
                    v.push_back(t);
                }
                j["comments"] = v;
            }

            if(subject_type == "music"){
                CSelection ep_list = doc.find("ul.line_list_music").nodeAt(0).find("li");
                string tag = "untagged";
                for(size_t i = 0; i < ep_list.nodeNum(); ++i){
                    if(ep_list.nodeAt(i).attribute("class") == "cat"){
                        tag = ep_list.nodeAt(i).ownText();
                        j["ep_list"][tag] = json::array();
                        continue;
                    }
                    if(j["ep_list"][tag] == nullptr)    j["ep_list"][tag] = json::object();
                    CNode item = ep_list.nodeAt(i).find("h6").nodeAt(0).find("a").nodeAt(0);
                    json v = json::object();
                    v["id"] = item.attribute("href").substr(strlen("/ep/"));
                    v["title"] = item.ownText();
                    j["ep_list"][tag].push_back(v);
                }
            }
        }else if(t_str == "user"){  // user page
            if(splited.size() == 3){    // user homepage
                if(doc.find("div.message").nodeNum() != 0){ // with message
                    j["message"] = doc.find("div.message").nodeAt(0).find("p.text").nodeAt(0).text();
                    j["URI"] = URI;
                    string json_str = "";
                    try {
                        json_str = j.dump();
                    }
                    catch (char *e) {
                        DEBUG_MSG(e);
                        free(e);
                    }
                    return json_str;
                }
                if(doc.find("div.tipIntro").nodeNum() != 0){
                    auto item = doc.find("div.tipIntro").nodeAt(0);
                    j["attention"] = {
                        {"title" ,item.find("h3").nodeAt(0).ownText()},
                        {"content",item.find("p.tip").nodeAt(0).ownText()}
                    };
                }
                if(doc.find("#headerProfile").nodeNum() == 1){  // header profile
                    auto item = doc.find("#headerProfile").nodeAt(0);
                    j["user"] = json::object();
                    j["user"]["name"] = item.find("div.inner").nodeAt(0).find("a").nodeAt(0).ownText();
                    j["user"]["avatar"] = \
                        BangumiAdaptor::ParseImageURI(item.find("a.avatar").nodeAt(0).find("span").nodeAt(0).attribute("style"),strlen("user"));
                    j["user"]["id"] = item.find("a.avatar").nodeAt(0).attribute("href").substr(strlen("/user/"));

                }
                if(doc.find("div.bio").nodeNum() == 1){         // self intro
                    auto item = doc.find("div.bio").nodeAt(0);
                    j["introduce"] = Data.substr(item.startPos(),item.endPos() - item.startPos());
                }
                if(doc.find("ul.timeline").nodeNum() == 1){     // timeline
                    CSelection ls = doc.find("ul.timeline").nodeAt(0).find("li");
                    std::vector<json> vec;
                    for(auto index = 0; index < ls.nodeNum(); ++index){
                        CNode item = ls.nodeAt(index);
                        json v = json::object();
                        v["time"] = item.find("small.time").nodeAt(0).ownText();
                        CNode feed = item.find("small.feed").nodeAt(0);
                        v["feed"] = feed.text();
                        CSelection lss = feed.find("a");
                        std::vector<json> links;            
                        for(auto i = 0; i < lss.nodeNum(); ++i){
                            CNode a = lss.nodeAt(i);
                            json t = json::object();
                            t["title"] = a.ownText();
                            t["link"] = a.attribute("href");
                            links.push_back(t);
                        }
                        v["links"] = links;     // fixme: 是用 "links" 么？
                        vec.push_back(v);
                    }
                    j["timeline"] = vec;
                }
                if(doc.find("#friend").nodeNum() == 1){         // friend list
                    CSelection ls = doc.find("#friend").nodeAt(0).find("dl");
                    std::vector<json> vec;
                    for(auto index = 0; index < ls.nodeNum(); ++index){
                        CNode item = ls.nodeAt(index);
                        json v = json::object();
                        v["nickname"] = item.find("dd").nodeAt(0).text();
                        v["avatar"] = this->ParseImageURI(item.find("span.avatarNeue").nodeAt(0).attribute("style"),strlen("user"));
                        v["id"] = item.find("a.avatar").nodeAt(0).attribute("href").substr(strlen("/user/"));
                        vec.push_back(v);
                    }
                    j["friends"] = vec;
                }
                if(doc.find("ul.network_service").nodeNum() == 1){  // SNS
                    CSelection ls = doc.find("ul.network_service").nodeAt(0).find("li");
                    std::vector<json> vec;
                    for(auto index = 0; index < ls.nodeNum(); ++index){
                        CNode item = ls.nodeAt(index);
                        json v = json::object();
                        v["host"] = item.find("span.service").nodeAt(0).ownText();
                        if(item.find("a.l").nodeNum() != 0){
                            v["link"] = item.find("a.l").nodeAt(0).attribute("href");
                            v["text"] = item.find("a.l").nodeAt(0).ownText();
                        }else{
                            v["text"] = item.find("span.tip").nodeAt(0).ownText();
                        }
                        vec.push_back(v);
                    }
                    j["sns"] = vec;
                }
                if(doc.find("div#music").nodeNum() == 1){       // Music list
                    j["music"] = this->ParseUserHomepagePictureList(doc.find("div#music").nodeAt(0),"张");
                }
                if(doc.find("div#anime").nodeNum() == 1){       // Anime list
                    j["anime"] = this->ParseUserHomepagePictureList(doc.find("div#anime").nodeAt(0),"部");
                }
                if(doc.find("div#book").nodeNum() == 1){        // Book list
                    j["book"] = this->ParseUserHomepagePictureList(doc.find("div#book").nodeAt(0),"本");
                }
                if(doc.find("div#game").nodeNum() == 1){        // Book list
                    j["game"] = this->ParseUserHomepagePictureList(doc.find("div#game").nodeAt(0),"部");
                }
                if(doc.find("div#real").nodeNum() == 1){        // TV Series list
                    j["real"] = this->ParseUserHomepagePictureList(doc.find("div#real").nodeAt(0),"部");
                }
                if(doc.find("div#group").nodeNum() == 1){       // Groups
                    CSelection ls = doc.find("div#group").nodeAt(0).find("li");
                    std::vector<json> vec;
                    for(auto index = 0; index < ls.nodeNum(); ++index){
                        CNode item = ls.nodeAt(index);
                        json v = json::object();
                        CNode tmpNode = item.find("img").nodeAt(0);
                        v["image"] = this->ParseImageURI(item.find("img").nodeAt(0).attribute("data-cfsrc"),strlen("icon"));
                        std::string id = item.find("a.l").nodeAt(0).attribute("href");
                        v["link"] = std::string(_BGM_PROTOCOL_) + "//" + _BGM_DOMAIN_ + id;
                        v["id"] = id.substr(strlen("/group/"));
                        std::string people = item.find("small.feed").nodeAt(0).ownText();
                        v["people"] = split(people," ")[0];
                        vec.push_back(v);
                    }
                    j["groups"] = vec;
                }
                if(doc.find("#mono").nodeNum() == 1){         // bookmarked characters list
                    CSelection ls = doc.find("#mono").nodeAt(0).find("dl");
                    std::vector<json> vec;
                    for(auto index = 0; index < ls.nodeNum(); ++index){
                        CNode item = ls.nodeAt(index);
                        json v = json::object();
                        v["name"] = item.find("dd").nodeAt(0).text();
                        v["avatar"] = this->ParseImageURI(item.find("img.avatar").nodeAt(0).attribute("data-cfsrc"),strlen("crt"));
                        CNode tmpNode = item.find("a.l").nodeAt(0);
                        auto list = split(item.find("a.l").nodeAt(0).attribute("href"),"/");
                        if(list.size() != 3){
                            DEBUG_MSG("Failed to split " << item.find("a.l").nodeAt(0).attribute("href"));
                            vec.push_back(v);
                            continue;
                        }
                        v["id"] = list[2];
                        v["type"] = list[1];
                        vec.push_back(v);
                    }
                    j["character"] = vec;
                }
            }
            
        }else{
            return "";
        }
    }
	string json_str = "";
	try {
		json_str = j.dump();
	}
	catch (char *e) {
		DEBUG_MSG(e);
		free(e);
	}
	
    return json_str;
}
