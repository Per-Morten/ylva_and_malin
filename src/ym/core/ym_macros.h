#pragma once

#define YM_INLINE [[gnu::always_inline]] inline
#define YM_NO_INLINE [[gnu::noinline]]
#define YM_PURE [[gnu::pure]]
#define YM_RESTRICT __restrict__
#define YM_UNUSED [[maybe_unused]]
#define YM_NO_DISCARD [[nodiscard]]
#define YM_LIKELY(x) __builtin_expect(!!(x), 1)
#define YM_UNLIKELY(x) __builtin_expect(!!(x), 0)


#if !defined(YM_ASSERT_THROW)
#define YM_NOEXCEPT noexcept
#else
#define YM_NOEXCEPT
#endif

#if defined(YM_ASSERT_THROW)
#include <stdexcept>
#endif

#if defined(YM_ASSERT_TERMINATE)
#define YM_ASSERT(expr, fmt, ...) \
if (YM_LIKELY(expr)) { }          \
else                              \
{                                 \
}
#elif defined(YM_ASSERT_THROW)
#define YM_ASSERT(expr, fmt, ...)           \
if (YM_LIKELY(expr)) { }                    \
else                                        \
{                                           \
    throw std::runtime_error(fmt);          \
}
#else
#define YM_ASSERT(expr, fmt, ...)
#endif

#define YM_PUSH_WARNING_TERMINATE   \
_Pragma("GCC diagnostic push")      \
_Pragma("GCC diagnostic ignored \"-Wterminate\"")


#define YM_POP_WARNING          \
_Pragma("GCC diagnostic pop")

