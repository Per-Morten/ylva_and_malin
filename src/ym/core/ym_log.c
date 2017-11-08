#include <ym_log.h>
#include <stdatomic.h>

static atomic_flag print_protector = ATOMIC_FLAG_INIT;

void
ym_log(FILE* file,
       const char* type,
       const char* color,
       const char* filepath,
       const char* func,
       const int line,
       const char* fmt,
       ...)
{
    va_list args1;
    va_start(args1, fmt);
    va_list args2;
    va_copy(args2, args1);
    size_t size = 1 + vsnprintf(NULL, 0, fmt, args1);
    char buffer[size];

    va_end(args1);
    vsnprintf(buffer, size, fmt, args2);
    va_end(args2);

    #ifdef WIN32
    const char* filename = strrchr(filepath, '\\');
    #else
    const char* filename = strrchr(filepath, '/');
    #endif
    ++filename;

    #ifndef WIN32
    while (atomic_flag_test_and_set_explicit(&print_protector,
                                             memory_order_acquire))
    {

    }
    #endif

    fprintf(file, "%s[%-5s]: %-15s: %-25s:%4d: %s%s\n",
            color, type,
            filename, func,
            line, buffer,
            YM_COLOR_RESET);

    fflush(file);

    #ifndef WIN32
    atomic_flag_clear_explicit(&print_protector,
                               memory_order_release);
    #endif
}
