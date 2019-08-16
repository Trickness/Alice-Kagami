#ifndef _KAGAMI_CONSOLE_HPP_
#define _KAGAMI_CONSOLE_HPP_

#include "../src/third-party/daniele77/cli/cli.h"
#include "../src/third-party/daniele77/cli/clilocalsession.h"

using namespace std;
using namespace cli;

/* 
class Plugin
{
public:
    Plugin(const string& _name) : name(_name) { cout << "Plugin " << name << " loaded" << endl; }
    virtual ~Plugin() { cout << "Plugin " << name << " unloaded" << endl; }
    const string& Name() const { return name; }
private:
    const string name;
};

using Factory = function<unique_ptr<Plugin>(Menu*)>;

class PluginRegistry
{
public:
    static PluginRegistry& Instance() { return instance; }
    void Register(const string& name, Factory factory) { plugins.push_back(make_pair(name, factory)); }
    void Print(ostream& out) const
    {
        for (auto& p: plugins)
            out << " - " << p.first << endl;
    }
    unique_ptr<Plugin> Create(const string& name, Menu* menu) const
    {
        for (auto& p: plugins)
            if (p.first == name)
                return p.second(menu);
        return {};
    }
private:
    static PluginRegistry instance;
    vector<pair<string, Factory>> plugins;
};


class Registration
{
public:
    Registration(const string& name, Factory factory)
    {
        PluginRegistry::Instance().Register(name, factory);
    }
};

class PluginContainer
{
public:
    static PluginContainer& Instance() { return instance; }
    void SetMenu(Menu& _menu) { menu = &_menu; }
    void Load(const string& plugin)
    {
        auto p = PluginRegistry::Instance().Create(plugin, menu);
        if (p)
            plugins.push_back(move(p));
    }
    void Unload(const string& plugin)
    {
        plugins.erase(
            remove_if(
                plugins.begin(),
                plugins.end(),
                [&](std::ostream& p){ return p->Name() == plugin; }
            ),
            plugins.end()
        );
    }
    void PrintLoaded(ostream& out) const
    {
        for (auto& p: plugins)
            out << " - " << p->Name() << endl;        
    }
private:
    static PluginContainer instance;
    vector<unique_ptr<Plugin>> plugins;
    Menu* menu;
};

template <typename T, const char* NAME>
class RegisteredPlugin : public Plugin
{
public:
    RegisteredPlugin() : Plugin(NAME)
    {
        const Registration& dummy = registration;
        do { (void)(dummy); } while (0); // silence unused variable warning
    }
private:
    static unique_ptr<Plugin> Create(Menu* menu) { return make_unique<T>(menu); }
    static Registration registration;
};

template <typename T, const char* NAME>
Registration RegisteredPlugin<T, NAME>::registration(NAME, &RegisteredPlugin<T, NAME>::Create);
*/


#endif