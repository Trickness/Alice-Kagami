#include "include/CastBook.hpp"
#include <boost/uuid/detail/md5.hpp>
#include <boost/algorithm/hex.hpp>

using boost::uuids::detail::md5;
using namespace std;

CastBook::CastBook(const char* BookLocation){
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
    
    result = sqlite3_exec(this->mBook, "CREATE TABLE IF NOT EXISTS 'Roster' ( \
                                        DOMAIN CHAR(64) PRIMARY KEY NOT NULL, \
                                        COOKIES TEXT,       \
                                        CUSTOM_INFO TEXT    \
    )", NULL, NULL, &sqliteErrorMsg);
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

void CastBook::PutRecord(const char* URI, const void* Buffer, size_t bytes){
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
    sqlite3_prepare(this->mBook, sql, strlen(sql),&stmt,NULL);
    sqlite3_bind_blob(stmt,1,Buffer,bytes,NULL);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

size_t CastBook::GetAllRecord(const char* URI, vector<unique_ptr<CastBookRecord>>& Records){
    sqlite3_stmt *stmt;
    char sql[9216];
    memset(sql, 0, 9216);
    sprintf(sql, "SELECT * FROM Caches WHERE URI = '%s'",URI);
    sqlite3_prepare(this->mBook, sql, strlen(sql), &stmt, NULL);
    if(sqlite3_step(stmt) != SQLITE_ROW){
        DEBUG_MSG("No cache found for URI[" << URI << "]");
        return 0;
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
            UID,                                \
            (char*)sqlite3_column_text(stmt,1), \
            timestamp,                          \
            sqlite3_column_blob(stmt,3),        \
            sqlite3_column_bytes(stmt,3)        \
        ));
        Records.push_back(std::move(record));
    }while(sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);
    return Records.size();
}

std::unique_ptr<CastBookRecord> CastBook::GetRecord(const char* URI){
    vector<unique_ptr<CastBookRecord>> Records;
    size_t result = this->GetAllRecord(URI,Records);
    if(result == 0){
        return nullptr;
    }else{
        unique_ptr<CastBookRecord> return_var;
        return_var.reset(Records[result-1].release());
        return return_var;
    }
}


string CastBook::GetRosterCookies(const char* Domain){
    sqlite3_stmt *stmt;
    char sql[256];
    memset(sql, 0, 256);
    sprintf(sql, "SELECT COOKIES FROM Roster WHERE DOMAIN = '%s'",Domain);
    sqlite3_prepare(this->mBook, sql, strlen(sql), &stmt, NULL);
    if(sqlite3_step(stmt) != SQLITE_ROW){
        DEBUG_MSG("No Cookies found for Domain[" << Domain << "]");
        return "";
    }
    string return_var = string((char*)sqlite3_column_text(stmt,0));
    sqlite3_finalize(stmt);
    return return_var;
}

string CastBook::GetRosterCustomInfo(const char* Domain){
    sqlite3_stmt *stmt;
    char sql[256];
    memset(sql, 0, 256);
    sprintf(sql, "SELECT CUSTOM_INFO FROM Roster WHERE DOMAIN = '%s'",Domain);
    sqlite3_prepare(this->mBook, sql, strlen(sql), &stmt, NULL);
    if(sqlite3_step(stmt) != SQLITE_ROW){
        DEBUG_MSG("No Cookies found for Domain[" << Domain << "]");
        return "";
    }
    string return_var = string((char*)sqlite3_column_text(stmt,0));
    sqlite3_finalize(stmt);
    return return_var;
}

void CastBook::SetRosterCookies(const char* Domain, const char* Cookies){
    sqlite3_stmt *stmt;
    char sql[9216];
    memset(sql, 0, 9216);
    sprintf(sql, "INSERT OR REPLACE INTO Roster(DOMAIN,COOKIES) VALUES ('%s',?)",Domain);
    sqlite3_prepare(this->mBook, sql, strlen(sql),&stmt,nullptr);
    sqlite3_bind_text(stmt,1,Cookies,strlen(Cookies),NULL);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void CastBook::AddRosterCookies(const char* Domain, const char* Cookies){
    // not in use
}


CastBookRecord::CastBookRecord(const char *UID, const char* URI, time_t t,const void* Data, size_t Bytes){
    memcpy(this->mUID,UID,32);
    this->mURI = new char[strlen(URI)+1];
    memset(this->mURI,0,strlen(URI)+1);
    strcpy(this->mURI,URI);
    this->mInclusionDatetime = t;
    this->mData = malloc(Bytes);
    memset(mData,0,Bytes);
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