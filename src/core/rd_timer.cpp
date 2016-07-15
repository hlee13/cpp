/*
*  File    : logger.cpp
*  Author  : lihan
*  Version : 1.0
*  Company : 
*  Contact : 
*  Date    : 2016-06-20 12:29:45
*/

#include "rd_timer.h"

namespace rd {

Timer::Timer() {
    gettimeofday(&beg_time, NULL);
}

int Timer::calc_span() {
    return (end_time.tv_sec - beg_time.tv_sec) * 1000000 + 
        (end_time.tv_usec - beg_time.tv_usec);
}

int Timer::reset() {
    return gettimeofday(&beg_time, NULL);
}

int Timer::cost_s() {
    gettimeofday(&end_time, NULL);
    return end_time.tv_sec - beg_time.tv_sec;
}

int Timer::cost_ms() {
    gettimeofday(&end_time, NULL);
    return calc_span() / 1000;
}

int Timer::cost_us() {
    gettimeofday(&end_time, NULL);
    return calc_span();
}

}
