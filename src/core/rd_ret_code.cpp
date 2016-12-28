/*
*  File    : rd_ret_code.cpp
*  Author  : 
*  Version : 
*  Company : 
*  Contact : 
*  Date    : 2016-07-30 11:20:24
*/

#include "rd_ret_code.h"

const char * errcode2errmsg(const ret_code code) {
    return code2msg[code];
}
