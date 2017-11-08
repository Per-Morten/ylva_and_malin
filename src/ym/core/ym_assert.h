#pragma once
#include <ym_macros.h>
#include <ym_error.h>
#include <ym_log.h>
#include <stdlib.h>

YM_EXTERN_CPP_BEGIN

///////////////////////////////////////////////////////////
/// \ingroup ym_core
///
/// \brief
///     Simple custom assert message that support
///     different functionality depending on build
///     settings.
///
/// \detailed
///     The YM_ASSERT has four different settings
///     based on build defines.
///
/// \par YM_ASSERT_TERMINATE
///     With this setting each failed assertion
///     will lead to a termination of the program.
///
/// \par YM_ASSERT_REPORT
///     With this setting each failed assertion
///     will be a warning that is printed to the window.
///
/// \par YM_ASSERT_ERRC
///     With this setting each failed assertion
///     will result in raising a global error.
///     This behavior should not be used to recover
///     from as it is mainly used to avoid termination
///     when testing.
///
/// \par No definitions
///     In the case where there are no definitions
///     the assert will be empty, and the checks
///     are optimized out.
///////////////////////////////////////////////////////////
#if defined(YM_ASSERT_TERMINATE)
#define YM_ASSERT(expr, ym_errc, fmt, ...)                          \
if (YM_LIKELY(expr)) { }                                            \
else                                                                \
{                                                                   \
    YM_ERROR("Assertion failure: " #expr ": " fmt, ##__VA_ARGS__);  \
}

#elif defined(YM_ASSERT_REPORT)
#define YM_ASSERT(expr, ym_errc, fmt, ...)                          \
if (YM_LIKELY(expr)) { }                                            \
else                                                                \
{                                                                   \
    YM_WARN("Assertion failure: " #expr ": " fmt, ##__VA_ARGS__);   \
}

#elif defined(YM_ASSERT_ERRC)
#define YM_ASSERT(expr, ym_errc, fmt, ...)                          \
if (YM_LIKELY(expr)) { }                                            \
else                                                                \
{                                                                   \
    ym_raise_error(ym_errc);                                        \
}

#else
#define YM_ASSERT(expr, ym_errc, fmt, ...)
#endif

YM_EXTERN_CPP_END
// Static assert does not get defined when compiling for windows
#ifdef WIN32
#define static_assert _Static_assert
#endif
