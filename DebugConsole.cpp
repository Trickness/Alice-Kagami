#include <iostream>
#include <memory>
#include <cstring>
#include <vector>
#include <iomanip>  // for setw
#include "include/AliceNoKagami.hpp"

#include "src/third-party/nlohmann/json.hpp"


using namespace std;
using json = nlohmann::json;

AliceNoKagami *Alice = new AliceNoKagami();

int main(){
    string uri;
    cin >> uri;
    cout << setw(4) <<  json::parse(Alice->GetParsedContentSync(uri.c_str(),Wonderland::CachePolicy::FIRST_FROM_CACHE));
}
