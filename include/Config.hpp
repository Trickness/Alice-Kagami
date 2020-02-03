#ifndef _CONFIG_HPP__
#define _CONFIG_HPP__
#include <cstring>
#include <iostream>
#include <thread>
#include <mutex>

#define DEBUG 1

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#ifdef DEBUG
#define DEBUG_MSG(MSG)\
    std::cout << "[" << __FILE__ << ":" << __FUNCTION__ << "] " << MSG << std::endl;
#else
#define DEBUG_MSG(MSG)
#endif

#define CHECK_PARAM_PTR(PARAM,RETURN)  \
    if (PARAM == nullptr){ DEBUG_MSG(__FUNCTION__ << " : " << __LINE__ << ": Null param ptr"); return RETURN; }

#define CHECK_PARAM_STR(PARAM,RETURN)  \
    if (PARAM.empty()){ DEBUG_MSG(__FUNCTION__ << " : " << __LINE__ << ": Empty param std::string"); return RETURN; }

#define THREAD_POOL_SIZE 0      // 0 to disable threadpool

#endif