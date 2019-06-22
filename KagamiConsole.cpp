#include <iostream>
#include "include/AliceNoKagami.hpp"

using namespace std;

int main(){
    AliceNoKagami *Alice = new AliceNoKagami();

    string result = Alice->GetParsedContentSync("https://bgm.tv/subject/276");
    DEBUG_MSG(result);

    delete Alice;
}
