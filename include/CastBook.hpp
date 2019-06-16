#ifndef _CAST_BOOK_
#define _CAST_BOOK_

/* Database of ACGNs */

#include <sqlite3.h>
#include <vector>
#include <memory>
#include "Config.hpp"


#define BOOK_LOCATION strcat(getenv("HOME"),"/.ACGNCastBook.db")

class CastBookRecord{
    public:
        CastBookRecord(const char *UID, const char* URI, time_t , const void* , size_t);
        ~CastBookRecord();
        
        const char* GetUID();
        const char* GetURI();
        time_t      GetInclusionDatetime();
        const void* GetData();
        size_t      GetSize();
    private:
        char  mUID[32];
        char* mURI;
        time_t mInclusionDatetime;
        void* mData;
        size_t mDataSize;
};

class CastBook{
    public:
        CastBook(const char* BookLocation=BOOK_LOCATION);
        ~CastBook();

        /* Save data to CastBook 
            throw sqlite3 error code on expection
        */
        void Put(const char* URI, const void* Buffer, size_t bytes);

        /* Get data from CastBook 
            return nullptr if no cache
        */
        CastBookRecord* Get(const char* URI);
        bool GetAll(const char* URI, std::vector<std::unique_ptr<CastBookRecord>>& Records);
    private:
        sqlite3*    mBook;
};

#endif