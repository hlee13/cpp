/*
*  File    : logger.h
*  Author  : 
*  Version : 1.0
*  Company : 
*  Contact : 
*  Date    : 2016-06-20 12:28:28
*/

#include <log4cplus/logger.h>                                                                                    
#include <log4cplus/loggingmacros.h>                                                                             
#include <log4cplus/helpers/loglog.h>                                                                            
#include <log4cplus/configurator.h>

#ifndef _LOGGER_H_
#define _LOGGER_H_

namespace rd {

extern log4cplus::Logger root;
int init_log(const char* log4cplus_properties_filename);
}

#define  INFO_LOG(fmt, ...)  LOG4CPLUS_INFO_FMT(rd::root, fmt, ##__VA_ARGS__) 
#define  WARN_LOG(fmt, ...)  LOG4CPLUS_WARN_FMT(rd::root, fmt, ##__VA_ARGS__)
#define DEBUG_LOG(fmt, ...) LOG4CPLUS_DEBUG_FMT(rd::root, fmt, ##__VA_ARGS__)
#define ERROR_LOG(fmt, ...) LOG4CPLUS_ERROR_FMT(rd::root, fmt, ##__VA_ARGS__)
#define FATAL_LOG(fmt, ...) LOG4CPLUS_FATAL_FMT(rd::root, fmt, ##__VA_ARGS__)
#define NOTICE_LOG(fmt, ...) LOG4CPLUS_NOTICE_FMT(rd::root, fmt, ##__VA_ARGS__)

#endif

