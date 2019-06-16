#ifndef _CONFIG_HPP_
#define _CONFIG_HPP_
#include <cstring>
#include <iostream>

#define DEBUG 1

#ifdef DEBUG
#define DEBUG_MSG(MSG)\
    std::cout << "[" << __FILE__ << "]" << MSG << std::endl;
#else
#define DEBUG_MSG(MSG)
#endif

#define CHECK_PARAM_PTR(PARAM,RETURN)  \
    if (PARAM == nullptr){ DEBUG_MSG(__FUNCTION__ << " : " << __LINE__ << ": Null param ptr"); return RETURN; }

#endif