#pragma once
#include <stddef.h>
#include <stdio.h>
#include <time.h>

#ifdef WIN32
#include <Windows.h>
#else
#include <sys/sysinfo.h>
#include <sys/time.h>
#endif

YM_INLINE
double
ym_clock_now()
{
    #ifdef WIN32
    LARGE_INTEGER frequency;
    LARGE_INTEGER time;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&time);

    // Multiply to avoid loss-of-precision
    time.QuadPart *= 1E6;
    time.QuadPart /= frequency.QuadPart;

    // Divide to convert from microseconds to seconds.
    return time.QuadPart / 1E6;

    #else
    struct timeval tp;
    gettimeofday(&tp, NULL);

    return (double)tp.tv_sec + (double)tp.tv_usec / 1E6;
    #endif
}
