#include <iostream>
#include <memory>
#include <cstring>
#include "include/AliceNoKagami.hpp"
#include "include/KagamiConsole.hpp"
#include <vector>

using namespace std;
using namespace cli;

//PluginRegistry PluginRegistry::instance;
//PluginContainer PluginContainer::instance;
AliceNoKagami *Alice = new AliceNoKagami();
            // command  help_message
vector<pair<string, string>> Helps;

void BuildHelp(){
    Helps.push_back(pair<string, string>("get","get - 获取URI指向的数据\n\nUSAGE: get <format> <uri>\n\n  format : 数据的返回格式\n            json -- 以JSON格式返回数据（需要Adaptor支持）\n            html -- 返回HTML数据\n     uri : 目标网址，例如 https://bgm.tv/\n\nEXAMPLE:\n  get json https://bgm.tv/\n\n"));
}

int main(){

    BuildHelp();


#if BOOST_VERSION < 106600
    boost::asio::io_service ioc;
#else
    boost::asio::io_context ioc;
#endif
    unique_ptr<Menu> rootMenu(new Menu("cli"));
    //PluginContainer::Instance().SetMenu(*rootMenu);
    
    rootMenu->Insert(
        "get", {"format", "uri"},
        [](std::ostream& out, const string& format, const string& uri){
            out << "GET " << uri << " with format "  << format << endl;
        },
        "Request GET"
    );
    rootMenu->Insert(
        "help",{"command"},
        [](std::ostream& out, const string& command){
            out <<  endl << "    Alice's Kagami help page" << endl  <<"---------------------------------" << endl;
            for(auto item:Helps){
                #ifdef __linux__
                if(strcasecmp(item.first.c_str(),command.c_str()) == 0){
                #else
                if(stricmp(item.first.c_str(),command.c_str()) == 0){
                #endif
                    out << item.second;
                    return;
                }
            }
            out << "No help topic for '"<< command << "'" << endl;
        },
        "show help"
    );
    Cli cli(std::move(rootMenu));
    cli.ExitAction([](std::ostream& out){
        delete Alice;
    });

    CliLocalTerminalSession localSession(cli, ioc, std::cout, 200);
    localSession.ExitAction(
        [&ioc](std::ostream& out) // session exit action
        {
            out << "Closing Kagami Console...\n";
            ioc.stop();
        }
    );
#if BOOST_VERSION < 106600
    boost::asio::io_service::work work(ioc);
#else
    auto work = boost::asio::make_work_guard(ioc);
#endif    
    ioc.run();

    return 0;
    //string result = Alice->GetParsedContentSync("https://bgm.tv/subject/276",Wonderland::CachePolicy::NEVER_FROM_CACHE);
    //DEBUG_MSG(result);

}
