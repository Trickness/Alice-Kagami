#ifndef _BANGUMI_ADAPTOR_HPP_
#define _BANGUMI_ADAPTOR_HPP_
/* Bangumi.tv & bgm.tv */

#include "include/WonderlandAdaptor.hpp"

#define _BGM_PROTOCOL_ "https:"
#define _BGM_DOMAIN_   "bgm.tv"

class BangumiAdaptor : public WonderlandAdaptor{
    public:
        BangumiAdaptor();
        ~BangumiAdaptor();

        bool CheckURI(const std::string &URI) const;
    protected:
        std::string ParseContent(std::string URI, const std::string &Data) const;

    private:
        static std::string ParseImageURI(std::string style_bg_image, int type){
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
          avatar_header[strlen("//lain.bgm.tv/pic//") + type] = 'l';        // begin from 0 change image to 'large'
          return std::string(_BGM_PROTOCOL_).append(avatar_header);
        }
};

#endif
