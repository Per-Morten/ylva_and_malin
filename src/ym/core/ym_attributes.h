#pragma once
#include <ym_macros.h>
YM_EXTERN_CPP_BEGIN

///////////////////////////////////////////////////////////
/// \ingroup ym_core
///
/// \brief
///     YM_INLINE is a platform independent macro
///     that forces a function to be inlined.
///////////////////////////////////////////////////////////
#define YM_INLINE __attribute__((always_inline)) inline

///////////////////////////////////////////////////////////
/// \ingroup ym_core
///
/// \brief
///     YM_NO_INLINE is a platform independent macro
///     that forces a function to never be inlined.
///////////////////////////////////////////////////////////
#define YM_NO_INLINE __attribute__((noinline))

///////////////////////////////////////////////////////////
/// \ingroup ym_core
///
/// \brief
///     YM_PURE is a platform independent macro
///     informing the compiler that a function is pure.
///////////////////////////////////////////////////////////
#define YM_PURE __attribute__((pure))

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
#define YM_UNUSED __attribute__((unused))

///////////////////////////////////////////////////////////
/// \ingroup ym_core
///
/// \brief
///     YM_NO_DISCARD is a platform independent macro
///     informing the compiler to warn if a return
///     value from a function is discarded.
///////////////////////////////////////////////////////////
#define YM_NO_DISCARD __attribute__((warn_unused_result))

///////////////////////////////////////////////////////////
/// \ingroup ym_core
///
/// \brief
///     YM_LIKELY is a platform independent macro
///     informing the compiler that in a branch
///     this is the path that is most likely.
///////////////////////////////////////////////////////////
#define YM_LIKELY(x) __builtin_expect(!!(x), 1)

///////////////////////////////////////////////////////////
/// \ingroup ym_core
///
/// \brief
///     YM_UNLIKELY is a platform independent macro
///     informing the compiler that in a branch
///     this is the path that is least likely.
///////////////////////////////////////////////////////////
#define YM_UNLIKELY(x) __builtin_expect(!!(x), 0)

YM_EXTERN_CPP_END
