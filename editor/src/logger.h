///////////////////////////////////////////////////////////////////////////////////////////////////
/// Logger.h
///////////////////////////////////////////////////////////////////////////////////////////////////
/// Description
///////////////////////////////////////////////////////////////////////////////////////////////////
/// Contains basic functionality for categorized logging to console.
/// The logging statements will either print to stdout or to stderr depending on
/// the category of the message.
///
/// Logging should be done through the different macros,
/// NOT through the log_internal function!
///
/// All messages will be following the specified format:
/// <category>: <file>: <function>: <line_number>: user specified message.
///
/// Example:
/// [INFO ]: main.cpp  : main      :  7: User specified message
///
/// The logs must be written with printf syntax, which can be found here:
/// http://en.cppreference.com/w/cpp/io/c/fprintf
///
/// note, you must use .c_str() or .data() when sending a std::string to "%s".
///
///////////////////////////////////////////////////////////////////////////////////////////////////
/// Example of usage:
/// #include <string>
/// #include "logger.h"
///
/// int main()
/// {
///     std::string str = "hey";
///     LOG_DEBUG("Logging debug: %s yas", str.c_str());
///     return 0;
/// }
///
/// Will return something like:
/// [DEBUG]: main.cpp  : main      :  7: Logging debug: hey yas
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#endif

#define LOG_COLOR_RESET "\033[0m"
#define LOG_COLOR_FG_BLACK "\033[0;30m"
#define LOG_COLOR_FG_RED "\033[0;31m"
#define LOG_COLOR_FG_GREEN "\033[0;32m"
#define LOG_COLOR_FG_YELLOW "\033[0;33m"
#define LOG_COLOR_FG_BLUE "\033[0;34m"
#define LOG_COLOR_FG_MAGENTA "\033[0;35m"
#define LOG_COLOR_FG_CYAN "\033[0;36m"
#define LOG_COLOR_FG_GREY "\033[0;37m"
#define LOG_COLOR_FG_WHITE "\033[0m"

#define LOG_COLOR_BG_BLACK "\033[0;40m"
#define LOG_COLOR_BG_RED "\033[0;41m"
#define LOG_COLOR_BG_GREEN "\033[0;42m"
#define LOG_COLOR_BG_YELLOW "\033[0;43m"
#define LOG_COLOR_BG_BLUE "\033[0;44m"
#define LOG_COLOR_BG_MAGENTA "\033[0;45m"
#define LOG_COLOR_BG_CYAN "\033[0;46m"
#define LOG_COLOR_BG_GREY "\033[0;47m"
#define LOG_COLOR_BG_WHITE "\033[0m"


///////////////////////////////////////////////////////////
/// \brief
///     Actual function which is called by log
///     macros.
///
/// \note
///     ATTENTION!
///     Do not use this function!
///     You are supposed to use the logger macros!
///////////////////////////////////////////////////////////
void
log_internal(FILE* file,
             const char* type,
             const char* color,
             const char* filepath,
             const char* func,
             const int line,
             const char* fmt,
             ...);

///////////////////////////////////////////////////////////
/// Convenience macro for letting the console "hang"
/// on windows systems.
///////////////////////////////////////////////////////////
#ifdef _WIN32
#define LOG_TERMINAL_PAUSE system("pause");
#else
#define LOG_TERMINAL_PAUSE
#endif

///////////////////////////////////////////////////////////
/// \brief
///     Prints the specified error to stderr and
///     terminates the program.
///
/// \detailed
///     This error is supposed to be used for
///     unrecoverable errors.
///////////////////////////////////////////////////////////
#define LOG_ERROR(fmt, ...)                                                                             \
{                                                                                                       \
    log_internal(stderr, "ERROR", LOG_COLOR_FG_RED, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__);  \
    LOG_TERMINAL_PAUSE;                                                                                 \
    std::exit(EXIT_FAILURE);                                                                            \
}

///////////////////////////////////////////////////////////
/// \brief
///     Prints the specified warning to stderr.
///     Use for non breaking situations.
///////////////////////////////////////////////////////////
#define LOG_WARN(fmt, ...) \
log_internal(stderr, "WARN", LOG_COLOR_FG_YELLOW, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__);

///////////////////////////////////////////////////////////
/// \brief
///     Prints debug information to stdout.
///
/// \detailed
///     Use this for debug related information,
///     can be turned off by defining LOG_NO_DEBUG
///     before including logger.h
///////////////////////////////////////////////////////////
#ifndef LOG_NO_DEBUG
#define LOG_DEBUG(fmt, ...) \
log_internal(stdout, "DEBUG", LOG_COLOR_FG_CYAN, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__);

#else
#define LOG_DEBUG(fmt, ...)
#endif

///////////////////////////////////////////////////////////
/// \brief
///     Prints regular info to stdout.
///////////////////////////////////////////////////////////
#define LOG_INFO(fmt, ...) \
log_internal(stdout, "INFO", LOG_COLOR_FG_WHITE, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__);

#ifdef _WIN32
inline
bool
log_internal_init_for_windows()
{
    const auto outputs = { STD_OUTPUT_HANDLE, STD_ERROR_HANDLE };
    for (const auto& item : outputs)
    {
        HANDLE handle = GetStdHandle(item);
        if (handle == INVALID_HANDLE_VALUE)
            return false;

        DWORD mode;
        if (!GetConsoleMode(handle, &mode))
            return false;

        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        if (!SetConsoleMode(handle, mode))
            return false;
    }

    return true;
}
#endif

inline
void
log_internal(std::FILE* file,
             const char* type,
             const char* color,
             const char* filepath,
             const char* func,
             const int line,
             const char* fmt,
             ...)
{
    // Enable virtual terminal in windows for text color
    #ifdef _WIN32
    static bool initialized = false;
    if (!initialized)
    {
        if (!log_internal_init_for_windows())
        {
            std::fprintf(stderr, "Could not enable virtual terminal processing, you will not get any terminal colors\n");
        }

        initialized = true;
    }
    #endif

    // Logging logic
    va_list args1;
    va_start(args1, fmt);
    va_list args2;
    va_copy(args2, args1);

    std::size_t size = 1 + std::vsnprintf(nullptr, 0, fmt, args1);
    std::vector<char> buffer(size);

    va_end(args1);
    std::vsnprintf(buffer.data(), size, fmt, args2);
    va_end(args2);

    #ifdef _WIN32
    const char* filename = std::strrchr(filepath, '\\');
    #else
    const char* filename = std::strrchr(filepath, '/');
    #endif

    filename = (filename)
             ? filename + 1 // Increment past '/' or '\\'
             : filepath;

    std::fprintf(file, "%s[%-5s]%s: %-10s: %-10s:%3d: %s\n",
                 color, type, LOG_COLOR_RESET,
                 filename, func,
                 line, buffer.data());

    std::fflush(file);
}
