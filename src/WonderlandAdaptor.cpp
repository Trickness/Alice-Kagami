#include "include/WonderlandAdaptor.hpp"
#include <vector>

using namespace std;

WonderlandAdaptor::WonderlandAdaptor(){
    this->mCastBook = new CastBook();
    DEBUG_MSG("Wonderland Adaptor Version 0.1 is loaded.");
}

WonderlandAdaptor::~WonderlandAdaptor(){
    delete this->mCastBook;
}

size_t WonderlandAdaptor::GetResource(const char* URI,void *&Buffer, short CacheStratagy){
    CHECK_PARAM_PTR(URI,0)
    if(CacheStratagy <= Wonderland::FIRST_FROM_CACHE){
        unique_ptr<CastBookRecord> result = this->mCastBook->Get(URI);
        if(result){
            Buffer = malloc(result->GetSize());
            memcpy(Buffer, result->GetData(), result->GetSize());
            return result->GetSize();
        }
        if(CacheStratagy == Wonderland::ONLY_FROM_CACHE){
            return 0;
        }
    }
    return 0;
}

void WonderlandAdaptor::PutResource(const char* URI, const void *Buffer, size_t Bytes){
    CHECK_PARAM_PTR(URI,)
    CHECK_PARAM_PTR(Buffer,)
    this->mCastBook->Put(URI,Buffer,Bytes);
}