#ifndef _BANGUMI_ADAPTOR_HPP_
#define _BANGUMI_ADAPTOR_HPP_
/* Bangumi.tv & bgm.tv */

#include "include/WonderlandAdaptor.hpp"
#include "src/third-party/gumbo-parser/Node.h"
#include "src/third-party/nlohmann/json.hpp"



#define _BGM_PROTOCOL_ "https:"
#define _BGM_DOMAIN_   "bgm.tv"
                                          //      1       2       3       4       5           6   
static std::vector<std::string> IntToTypeStr = {"book","anime","music", "game", "unknown", "real"};
class BangumiAdaptor : public WonderlandAdaptor{
    public:
        BangumiAdaptor();
        ~BangumiAdaptor();

        bool CheckURI(const std::string &URI) const;
        void SegFaultHandler_Parse(void);
    protected:
        std::string ParseContent(std::string URI, const std::string &Data) const;

    private:
        static std::string ParseImageURI(std::string style_bg_image, int type){
            // type --> length of type:
            //              example : type = strlen("cover")
          std::string avatar_header;
          if(style_bg_image.find("//") != std::string::npos){
              avatar_header = style_bg_image.substr(style_bg_image.find_first_of("/"));
          }else{
              return std::string(_BGM_PROTOCOL_).append("//").append(_BGM_DOMAIN_).append("/img/info_only.png'");
          }

          if(avatar_header.find_first_of('?') == std::string::npos){
              avatar_header = avatar_header.substr(0,avatar_header.find_first_of('\''));
          }else{
              avatar_header = avatar_header.substr(0,avatar_header.find_first_of('?'));
          }
          avatar_header[strlen("//lain.bgm.tv/pic//") + type] = 'l';        
          return std::string(_BGM_PROTOCOL_).append(avatar_header);
        }
        static nlohmann::json ParseUserHomepagePictureList(CNode d, std::string separator);
        static nlohmann::json ParseInfobox(CNode d);
        static nlohmann::json ParseSlimSubject(CNode d);
        static nlohmann::json ParseSlimCharacter(CNode d);
        static nlohmann::json ParseCrtCommentList(CNode d,const std::string &Data);
        static nlohmann::json ParseCrtCommentListItem(CNode d,const std::string &Data);

};

#endif
