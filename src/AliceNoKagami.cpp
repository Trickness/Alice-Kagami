#include "include/AliceNoKagami.hpp"
using namespace std;

AliceNoKagami::AliceNoKagami(){
    this->mAdaptor = new WonderlandAdaptor();
    void* Buffer = nullptr;
    this->mAdaptor->GetResource("https://blog.csdn.net/Dawn_sf5561?uisss",Buffer);
    DEBUG_MSG("Alice's Kagami Version 0.1");
}

AliceNoKagami::~AliceNoKagami(){
    delete this->mAdaptor;
}

