#include "include/AliceNoKagami.hpp"
using namespace std;

AliceNoKagami::AliceNoKagami(){
    this->mAdaptor = new WonderlandAdaptor();
    DEBUG_MSG("Alice's Kagami Version 0.1");
}

AliceNoKagami::~AliceNoKagami(){
    delete this->mAdaptor;
}

