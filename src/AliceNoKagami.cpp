#include "include/AliceNoKagami.hpp"
using namespace std;

void testCallback(int taskID,WonderlandAdaptor::Status status, void* Data, size_t t){
    DEBUG_MSG((char*)Data);
    DEBUG_MSG(taskID);

    free(Data);
}

AliceNoKagami::AliceNoKagami(){
    this->mAdaptor = new WonderlandAdaptor();
    void* Buffer = nullptr;
    size_t bytes = this->mAdaptor->GetResourceSync("https://bgm.tv",  \
                                    Buffer, \
                                    WonderlandAdaptor::CachePolicy::FIRST_FROM_CACHE   \
                                    );
                                    //std::bind(testCallback,1,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
    DEBUG_MSG((char*)Buffer);
    free(Buffer);
    //this->mAdaptor->PutResource("ht561?uisss\"end","Hell\'o'", 8);
    DEBUG_MSG("Alice's Kagami Version 0.1");
}

AliceNoKagami::~AliceNoKagami(){
    delete this->mAdaptor;
}

