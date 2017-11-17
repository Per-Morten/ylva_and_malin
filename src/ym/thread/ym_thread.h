#pragma once
#include <ym_core.h>

#ifdef WIN32
#include <windows.h>
typedef HANDLE ym_thread_handle;
#else
#include <pthread.h>
typedef pthread_t ym_thread_handle;
#endif

typedef struct
{
    u32 id;
    ym_thread_handle _handle; // Do not touch!, internal, platform dependent.
} ym_thread;

typedef void(*ym_thread_routine)(void*);

ym_thread
ym_thread_create(ym_thread_routine routine,
                 void* arg);

void
ym_thread_join(ym_thread* thread);
