#include "include/CastBook.hpp"
#include <boost/uuid/detail/md5.hpp>
#include <boost/algorithm/hex.hpp>



using boost::uuids::detail::md5;
using namespace std;

CastBook::CastBook(const char* BookLocation){
    DEBUG_MSG("Load Castbook from " << BookLocation);
    int result = sqlite3_open_v2(BookLocation, &(this->mBook),SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,NULL);
    if(result != SQLITE_OK){
        DEBUG_MSG("Failed to open CastBook in [" << BookLocation << "] with sqlite3 error code " << result)
    }
    char *sqliteErrorMsg = NULL;

    result = sqlite3_exec(this->mBook,"CREATE TABLE IF NOT EXISTS 'Caches'(\
                                UID     CHAR(32) PRIMARY KEY NOT NULL,\
                                URI     TEXT    NOT NULL,\
                                INCLUSION_DATE DATETIME NOT NULL,\
                                DATA    BLOB NOT NULL);",NULL,NULL,&sqliteErrorMsg);
    if(result != SQLITE_OK){
        DEBUG_MSG(sqliteErrorMsg);
        throw(result);
    }
}

CastBook::~CastBook(){
    if(this->mBook != nullptr){
        sqlite3_close(this->mBook);
    }
}

void CastBook::Put(const char* URI, const void* Buffer, size_t bytes){
    #ifdef DEBUG
        if(Buffer == nullptr or URI == nullptr){
            DEBUG_MSG( ":" << __FUNCTION__ <<  __LINE__ << " null input!!");
            throw(0);
        }
    #endif
    md5 hash;
    hash.process_bytes(Buffer,bytes);
    hash.process_bytes(URI,strlen(URI));
    md5::digest_type digest;
    hash.get_digest(digest);
    const auto charDigest = reinterpret_cast<const char *>(&digest);
    std::string MD5SUM;
    boost::algorithm::hex(charDigest, charDigest + sizeof(md5::digest_type), std::back_inserter(MD5SUM));
    
    sqlite3_stmt *stmt;
    char sql[9216];
    memset(sql, 0, 9216);
    sprintf(sql, "INSERT INTO Caches(UID, URI, DATA) SELECT '%s', '%s', ? WHERE NOT EXISTS(SELECT 1 FROM Caches WHERE UID = '%s' AND URI = '%s')",MD5SUM.c_str(), URI, MD5SUM.c_str(),URI);
    DEBUG_MSG(sql);
    sqlite3_prepare(this->mBook, sql, strlen(sql),&stmt,NULL);
    sqlite3_bind_blob(stmt,1,Buffer,bytes,NULL);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

bool CastBook::GetAll(const char* URI, vector<unique_ptr<CastBookRecord>>& Records){
    #ifdef DEBUG
        if(URI == nullptr){
            DEBUG_MSG( ":" << __FUNCTION__ <<  __LINE__ << " null input!!");
            throw(0);
        }
    #endif
    sqlite3_stmt *stmt;
    char sql[9216];
    memset(sql, 0, 9216);
    sprintf(sql, "SELECT * FROM Caches WHERE URI = '%s'",URI);
    sqlite3_prepare(this->mBook, sql, strlen(sql), &stmt, NULL);
    if(sqlite3_step(stmt) != SQLITE_ROW){
        DEBUG_MSG("No cache found for URI[" << URI << "]");
        return false;
    }
    char UID[32];
    time_t timestamp;
    tm tm_;
    size_t bytes;
    do{
        memcpy(UID,sqlite3_column_text(stmt,0),32);        
        strptime((const char*)sqlite3_column_text(stmt,2),"%Y-%m-%d %H:%M:%S", &tm_);
        timestamp = mktime(&tm_);
        unique_ptr<CastBookRecord> record(new CastBookRecord(\
            UID,    \
            (char*)sqlite3_column_text(stmt,1),    \
            timestamp,  \
            sqlite3_column_text(stmt,3),    \
            sqlite3_column_bytes(stmt,3)    \
        ));

        /* unique_ptr<CastBookRecord> record(new CastBookRecord(   \
            static_cast<char*>(sqlite3_column_text(stmt, 1)),   \
            sqlite3_column_text(stmt, 2),   \
            sqlite3_column_int64(stmt, 3),  \
            sqlite3_column_blob(stmt, 4),   \
            sqlite3_column_bytes(stmt, 4)));*/
    }while(sqlite3_step(stmt) == SQLITE_ROW);
    return true;
}


CastBookRecord::CastBookRecord(const char *UID, const char* URI, time_t t,const void* Data, size_t Bytes){
    memcpy(this->mUID,UID,32);
    this->mURI = new char[strlen(URI)+1];
    memset(this->mURI,0,strlen(URI)+1);
    strcpy(this->mURI,URI);
    this->mInclusionDatetime = t;
    this->mData = malloc(Bytes);
    memcpy(this->mData,Data,Bytes);
    this->mDataSize = Bytes;
}

CastBookRecord::~CastBookRecord(){
    delete[] this->mURI;
    free(this->mData);
}

const char* CastBookRecord::GetUID(){
    return this->mUID;
}

const char* CastBookRecord::GetURI(){
    return this->mURI;
}

time_t CastBookRecord::GetInclusionDatetime(){
    return this->mInclusionDatetime;
}

const void* CastBookRecord::GetData(){
    return this->mData;
}

size_t CastBookRecord::GetSize(){
    return this->mDataSize;
}