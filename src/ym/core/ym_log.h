#pragma once
#include <ym_macros.h>
#include <ym_attributes.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

YM_EXTERN_CPP_BEGIN

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
///     Make lock-free thread safe.
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
///////////////////////////////////////////////////////////
#define YM_ERROR(fmt, ...) \
ym_log(stderr, "ERROR", YM_COLOR_FG_RED, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__); \
exit(EXIT_FAILURE);

///////////////////////////////////////////////////////////
/// \ingroup ym_core
///
/// \brief
///     Prints the specified warning to stderr.
///////////////////////////////////////////////////////////
#define YM_WARN(fmt, ...) \
ym_log(stderr, "WARN", YM_COLOR_FG_YELLOW, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__);

///////////////////////////////////////////////////////////
/// \ingroup ym_core
///
/// \brief
///     Prints debug information to stdout.
///////////////////////////////////////////////////////////
#define YM_DEBUG(fmt, ...) \
ym_log(stdout, "DEBUG", YM_COLOR_FG_CYAN, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__);

///////////////////////////////////////////////////////////
/// \ingroup ym_core
///
/// \brief
///     Prints regular info to stdout.
///////////////////////////////////////////////////////////
#define YM_INFO(fmt, ...) \
ym_log(stdout, "INFO", YM_COLOR_FG_WHITE, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__);

YM_EXTERN_CPP_END
