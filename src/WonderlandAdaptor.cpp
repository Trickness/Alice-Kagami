#include "include/WonderlandAdaptor.hpp"
#include <vector>

WonderlandAdaptor::WonderlandAdaptor(){
    this->mCastBook = new CastBook();
    DEBUG_MSG("Wonderland Adaptor Version 0.1 is loaded.");
    /*char *Buffer = new char[32];
    memset(Buffer,0,32);
    strcpy(Buffer,"ss23ss");
    this->mCastBook->Put("https://blog.csdn.net/Dawn_sf5561?uisss",Buffer, 32);
    delete[] Buffer;*/
    std::vector<std::unique_ptr<CastBookRecord>> Records;
    this->mCastBook->GetAll("https://blog.csdn.net/Dawn_sf5561?uisss",Records);
}

WonderlandAdaptor::~WonderlandAdaptor(){
    delete this->mCastBook;
}