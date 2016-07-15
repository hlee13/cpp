/*
*  File    : logger.cpp
*  Author  : lihan
*  Version : 1.0
*  Company : 
*  Contact : 
*  Date    : 2016-06-20 12:29:45
*/

#include <iostream>

#include "rd_logger.h"

namespace rd {

log4cplus::Logger root = log4cplus::Logger::getRoot();

int init_log(const char* log_conf_path) {
    log4cplus::initialize();                                                     

    try {                                                                        
        new log4cplus::ConfigureAndWatchThread(log_conf_path, 3 * 1000);         
    } catch(...) {                                                               
        return 1;                                                                
    } 
    return 0;
}

}
