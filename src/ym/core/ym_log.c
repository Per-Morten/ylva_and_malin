#include <ym_log.h>

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

    #if defined(_MSC_VER)
    const char* filename = strrchr(filepath, '\\');
    #else
    const char* filename = strrchr(filepath, '/');
    #endif
    ++filename;

    fprintf(file, "%s[%-5s]: %-10s: %-25s:%4d: %s%s\n",
            color, type,
            filename, func,
            line, buffer,
            YM_COLOR_RESET);

    fflush(file);
}
