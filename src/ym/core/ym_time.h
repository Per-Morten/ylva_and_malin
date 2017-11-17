#pragma once
#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <sys/sysinfo.h>
#include <sys/time.h>

inline
double
ym_clock_now()
{
    #ifdef WIN32

    #else
    struct timeval tp;
    gettimeofday(&tp, NULL);

    return (double)tp.tv_sec + (double)tp.tv_usec / 1E6;
    #endif
}
