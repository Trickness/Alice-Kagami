#ifndef _CONFIG_HPP__
#define _CONFIG_HPP__
#include <cstring>
#include <iostream>

#define DEBUG 1

#ifdef DEBUG
#define DEBUG_MSG(MSG)\
    std::cout << "[" << __FILE__ << ":" << __FUNCTION__ << "] " << MSG << std::endl;
#else
#define DEBUG_MSG(MSG)
#endif

#define CHECK_PARAM_PTR(PARAM,RETURN)  \
    if (PARAM == nullptr){ DEBUG_MSG(__FUNCTION__ << " : " << __LINE__ << ": Null param ptr"); return RETURN; }

#define THREAD_POOL_SIZE 0      // 0 to disable threadpool

#endif