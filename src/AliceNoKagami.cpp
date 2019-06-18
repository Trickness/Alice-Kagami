#include "include/AliceNoKagami.hpp"
using namespace std;

void testCallback(WonderlandAdaptor::Status status, const char* str, size_t t){
    DEBUG_MSG(str);
}

AliceNoKagami::AliceNoKagami(){
    this->mAdaptor = new WonderlandAdaptor();
    void* Buffer = nullptr;
    this->mAdaptor->GetResource("https://blog.csdn.'/D''aw'''n_sf'5561?uisss",Buffer,WonderlandAdaptor::CachePolicy::NEVER_FROM_CACHE,testCallback);
    
    //this->mAdaptor->PutResource("ht561?uisss\"end","Hell\'o'", 8);
    DEBUG_MSG("Alice's Kagami Version 0.1");
}

AliceNoKagami::~AliceNoKagami(){
    delete this->mAdaptor;
}

