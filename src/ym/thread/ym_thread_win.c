#include <ym_thread.h>

static ym_atomic_i32 ym_thread_latest_id;

typedef struct
{
    ym_thread_routine routine;
    void* arg;
} ym_thread_windows_arg;

static DWORD WINAPI
win_wrapper(LPVOID lpParameter)
{
    ym_thread_windows_arg* args = lpParameter;

    args->routine(args->arg);

    ExitThread(0);
}


ym_thread
ym_thread_create(ym_thread_routine routine,
                 void* arg)
{
    YM_ASSERT(routine,
              ym_errc_invalid_input,
              "Routine must not be NULL");

    ym_thread_windows_arg win_args =
    {
        .routine = routine,
        .arg = arg,
    };

    ym_thread thread =
    {
        .id = ym_atomic_i32_add(&ym_thread_latest_id, 1),
        ._handle = CreateThread(NULL, 0,
                                win_wrapper, &win_args,
                                0, NULL),
    };

    if (!thread._handle)
        YM_ERROR("Could not create thread: %lu", GetLastError());

    return thread;
}

void
ym_thread_join(ym_thread* thread)
{
    YM_ASSERT(thread,
              ym_errc_invalid_input,
              "Thread must not be NULL");

    WaitForSingleObject(thread->_handle, INFINITE);
    CloseHandle(thread->_handle);
}
