/*
*  File    : logger.h
*  Author  : lihan
*  Version : 1.0
*  Company : 
*  Contact : 
*  Date    : 2016-06-20 12:28:28
*/

#ifndef _TIMER_H_
#define _TIMER_H_

#include <sys/time.h>

namespace rd {

class Timer {
protected:
    // unit : us, microseconds
    int calc_span();
public:
    Timer();

    int reset();
    int cost_s();
    int cost_ms();
    int cost_us();
private:
    struct timeval beg_time;
    struct timeval end_time;
};

}

#endif

