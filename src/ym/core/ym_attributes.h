#pragma once
// Setup checks for builtin features, attributes and extensions.
// See: https://clang.llvm.org/docs/LanguageExtensions.html

#ifndef __has_builtin
#define __has_builtin(x) 0
#endif

#ifndef __has_feature
#define __has_feature(x) 0
#endif

#ifndef __has_extension
#define __has_extension __has_feature
#endif

#ifndef __has_attribute
#define __has_attribute(x) 0
#endif

///////////////////////////////////////////////////////////
/// \ingroup ym_core
///
/// \brief
///     YM_INLINE is a platform independent macro
///     that forces a function to be inlined.
///////////////////////////////////////////////////////////
#if __has_attribute(always_inline)
#define YM_INLINE __attribute__((always_inline)) inline
#else
#define YM_INLINE
#endif

///////////////////////////////////////////////////////////
/// \ingroup ym_core
///
/// \brief
///     YM_NO_INLINE is a platform independent macro
///     that forces a function to never be inlined.
///////////////////////////////////////////////////////////
#if __has_attribute(noinline)
#define YM_NO_INLINE __attribute__((noinline))
#else
#define YM_NO_INLINE
#endif

///////////////////////////////////////////////////////////
/// \ingroup ym_core
///
/// \brief
///     YM_PURE is a platform independent macro
///     informing the compiler that a function is pure.
///////////////////////////////////////////////////////////
#if __has_attribute(pure)
#define YM_PURE __attribute__((pure))
#else
#define YM_PURE
#endif

///////////////////////////////////////////////////////////
/// \ingroup ym_core
///
/// \brief
///     YM_RESTRICT is a platform independent macro
///     wrapper for C99 restrict functionality.
///////////////////////////////////////////////////////////
#define YM_RESTRICT restrict

///////////////////////////////////////////////////////////
/// \ingroup ym_core
///
/// \brief
///     YM_UNUSED is a platform independent macro
///     informing the compiler that a function/variable
///     is intentionally unused.
///////////////////////////////////////////////////////////
#if __has_attribute(unused)
#define YM_UNUSED __attribute__((unused))
#else
#define YM_UNUSED
#endif

///////////////////////////////////////////////////////////
/// \ingroup ym_core
///
/// \brief
///     YM_NO_DISCARD is a platform independent macro
///     informing the compiler to warn if a return
///     value from a function is discarded.
///////////////////////////////////////////////////////////
#if __has_attribute(warn_unused_result)
#define YM_NO_DISCARD __attribute__((warn_unused_result))
#else
#define YM_NO_DISCARD
#endif

#if __has_attribute(aligned)
#define YM_ALIGNED(x) __attribute__((aligned(x)))
#else
#define YM_ALIGNED
#pragma message "__attribute__ aligned not supported on platform, can destroy atomic operations!"
#endif

///////////////////////////////////////////////////////////
/// \ingroup ym_core
///
/// \brief
///     YM_LIKELY is a platform independent macro
///     informing the compiler that in a branch
///     this is the path that is most likely.
///////////////////////////////////////////////////////////
// Having issues with this in release on windows.
#if __has_builtin(__builtin_expect) && !defined(WIN32)
#define YM_LIKELY(x) __builtin_expect(!!(x), 1)
#else
#define YM_LIKELY(x) (x)
#endif

///////////////////////////////////////////////////////////
/// \ingroup ym_core
///
/// \brief
///     YM_UNLIKELY is a platform independent macro
///     informing the compiler that in a branch
///     this is the path that is least likely.
///////////////////////////////////////////////////////////
// Having issues with this in release on windows.
#if __has_builtin(__builtin_expect) && !defined(WIN32)
#define YM_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define YM_UNLIKELY(x) (x)
#endif

