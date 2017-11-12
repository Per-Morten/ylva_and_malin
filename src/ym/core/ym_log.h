#pragma once
#include <ym_attributes.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifdef WIN32
#define YM_COLOR_RESET ""
#define YM_COLOR_FG_BLACK ""
#define YM_COLOR_FG_RED ""
#define YM_COLOR_FG_GREEN ""
#define YM_COLOR_FG_YELLOW ""
#define YM_COLOR_FG_BLUE ""
#define YM_COLOR_FG_MAGENTA ""
#define YM_COLOR_FG_CYAN ""
#define YM_COLOR_FG_GREY ""
#define YM_COLOR_FG_WHITE ""

#define YM_COLOR_BG_BLACK ""
#define YM_COLOR_BG_RED ""
#define YM_COLOR_BG_GREEN ""
#define YM_COLOR_BG_YELLOW ""
#define YM_COLOR_BG_BLUE ""
#define YM_COLOR_BG_MAGENTA ""
#define YM_COLOR_BG_CYAN ""
#define YM_COLOR_BG_GREY ""
#define YM_COLOR_BG_WHITE ""

#else
#define YM_COLOR_RESET "\033[0m"
#define YM_COLOR_FG_BLACK "\033[0;30m"
#define YM_COLOR_FG_RED "\033[0;31m"
#define YM_COLOR_FG_GREEN "\033[0;32m"
#define YM_COLOR_FG_YELLOW "\033[0;33m"
#define YM_COLOR_FG_BLUE "\033[0;34m"
#define YM_COLOR_FG_MAGENTA "\033[0;35m"
#define YM_COLOR_FG_CYAN "\033[0;36m"
#define YM_COLOR_FG_GREY "\033[0;37m"
#define YM_COLOR_FG_WHITE "\033[0m"

#define YM_COLOR_BG_BLACK "\033[0;40m"
#define YM_COLOR_BG_RED "\033[0;41m"
#define YM_COLOR_BG_GREEN "\033[0;42m"
#define YM_COLOR_BG_YELLOW "\033[0;43m"
#define YM_COLOR_BG_BLUE "\033[0;44m"
#define YM_COLOR_BG_MAGENTA "\033[0;45m"
#define YM_COLOR_BG_CYAN "\033[0;46m"
#define YM_COLOR_BG_GREY "\033[0;47m"
#define YM_COLOR_BG_WHITE "\033[0m"
#endif



///////////////////////////////////////////////////////////
/// \brief
///     Actual function which is called by log
///     macros.
///
/// \note
///     Do not use this function!
///     You are supposed to use the logger macros.
///
/// \todo
///     Add timing functionality.
///
/// \todo
///     Make proper buffered logger which does not
///     require a lock.
///     But rather logs to a specified location,
///     and is later printed out by the debug thread.
///////////////////////////////////////////////////////////
void
ym_log(FILE* file,
       const char* type,
       const char* color,
       const char* filepath,
       const char* func,
       const int line,
       const char* fmt,
       ...);

///////////////////////////////////////////////////////////
/// \ingroup ym_core
///
/// \brief
///     Prints the specified error to stderr and
///     terminates the program.
///
/// \detailed
///     This error is supposed to be used for
///     unrecoverable errors.
///
/// \note
///     Can be called concurrently.
///////////////////////////////////////////////////////////
#ifdef WIN32
#define TERMINAL_PAUSE system("pause");
#else
#define TERMINAL_PAUSE
#endif

#define YM_ERROR(fmt, ...)                                                                      \
{                                                                                               \
    ym_log(stderr, "ERROR", YM_COLOR_FG_RED, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__); \
    TERMINAL_PAUSE;                                                                             \
    exit(EXIT_FAILURE);                                                                         \
}

///////////////////////////////////////////////////////////
/// \ingroup ym_core
///
/// \brief
///     Prints the specified warning to stderr.
///
/// \note
///     Can be called concurrently.
///////////////////////////////////////////////////////////
#define YM_WARN(fmt, ...) \
ym_log(stderr, "WARN", YM_COLOR_FG_YELLOW, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__);

///////////////////////////////////////////////////////////
/// \ingroup ym_core
///
/// \brief
///     Prints debug information to stdout.
///
/// \note
///     Can be called concurrently.
///////////////////////////////////////////////////////////
#define YM_DEBUG(fmt, ...) \
ym_log(stdout, "DEBUG", YM_COLOR_FG_CYAN, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__);

///////////////////////////////////////////////////////////
/// \ingroup ym_core
///
/// \brief
///     Prints regular info to stdout.
///
/// \note
///     Can be called concurrently.
///////////////////////////////////////////////////////////
#define YM_INFO(fmt, ...) \
ym_log(stdout, "INFO", YM_COLOR_FG_WHITE, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__);
