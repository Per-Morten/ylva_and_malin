#pragma once
#include <stddef.h>
#include <stdio.h>
#include <time.h>

#ifdef WIN32
#else
#include <sys/sysinfo.h>
#include <sys/time.h>
#endif

YM_INLINE
double
ym_clock_now()
{
    #ifdef WIN32
    return 0.0;
    #else
    struct timeval tp;
    gettimeofday(&tp, NULL);

    return (double)tp.tv_sec + (double)tp.tv_usec / 1E6;
    #endif
}
