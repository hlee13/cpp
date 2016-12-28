/*
*  File    : rd_ret_code.h
*  Author  : 
*  Version : 
*  Company : 
*  Contact : 
*  Date    : 2016-07-30 11:20:24
*/

#ifndef _RD_RET_CODE_H_
#define _RD_RET_CODE_H_

enum ret_code {
    OK,
    ERROR,

    NOEXIST,

    CONNECT_FAILED,
    TIMEOUT,

    UNKNOWN
};

static const char* code2msg[] {
    "OK",
    "ERROR",
    "NOEXIST"
    "CONNECT_FAILED",
    "TIMEOUT",
    "UNKNOWN"
};

const char * errcode2errmsg(const ret_code);

#endif//_RD_RET_CODE_H_

