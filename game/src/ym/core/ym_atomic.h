#pragma once

#include <stdatomic.h>
#include <stdint.h>
#include <assert.h>
#include <limits.h>
#include <ym_assert.h>

///////////////////////////////////////////////////////////
// Core assertions for ym_atomics.
///////////////////////////////////////////////////////////
// Atomics should be its own project, does not belong in here.
#ifndef WIN32
static_assert(CHAR_BIT == 8, "char is not 8 bit on this platform");
static_assert(sizeof(atomic_schar)  * CHAR_BIT ==  8, "atomic_schar is not 8 bit");
static_assert(sizeof(atomic_short)  * CHAR_BIT == 16, "atomic_short is not 16 bit");
static_assert(sizeof(atomic_int)    * CHAR_BIT == 32, "atomic_int is not 32 bit");
static_assert(sizeof(atomic_long)   * CHAR_BIT == 64, "atomic_long is not 64 bit");

static_assert(sizeof(atomic_uchar)  * CHAR_BIT ==  8, "atomic_uchar is not 8 bit");
static_assert(sizeof(atomic_ushort) * CHAR_BIT == 16, "atomic_ushort is not 16 bit");
static_assert(sizeof(atomic_uint)   * CHAR_BIT == 32, "atomic_int is not 32 bit");
static_assert(sizeof(atomic_ulong)  * CHAR_BIT == 64, "atomic_long is not 64 bit");

static_assert(ATOMIC_BOOL_LOCK_FREE     == 2, "ATOMIC_BOOL is not lock free");
static_assert(ATOMIC_CHAR_LOCK_FREE     == 2, "ATOMIC_CHAR is not lock free");
static_assert(ATOMIC_CHAR16_T_LOCK_FREE == 2, "ATOMIC_CHAR16_T is not lock free");
static_assert(ATOMIC_CHAR32_T_LOCK_FREE == 2, "ATOMIC_CHAR32_T is not lock free");
static_assert(ATOMIC_WCHAR_T_LOCK_FREE  == 2, "ATOMIC_WCHAR_T is not lock free");
static_assert(ATOMIC_SHORT_LOCK_FREE    == 2, "ATOMIC_SHORT is not lock free");
static_assert(ATOMIC_INT_LOCK_FREE      == 2, "ATOMIC_INT is not lock free");
static_assert(ATOMIC_LONG_LOCK_FREE     == 2, "ATOMIC_LONG is not lock free");
static_assert(ATOMIC_LLONG_LOCK_FREE    == 2, "ATOMIC_LLONG is not lock free");
static_assert(ATOMIC_POINTER_LOCK_FREE  == 2, "ATOMIC_POINTER is not lock free");
#endif
///////////////////////////////////////////////////////////
/// \typedef atomic_int8_t
/// \ingroup ym_core
/// \brief
///     Atomic signed 8-bit integer.
///     Supplied because C11 does not mandate fixed
///     width atomic support.
///
/// \typedef atomic_int16_t
/// \ingroup ym_core
/// \brief
///     Atomic signed 16-bit integer.
///     Supplied because C11 does not mandate fixed
///     width atomic support.
///
/// \typedef atomic_int32_t
/// \ingroup ym_core
/// \brief
///     Atomic signed 32-bit integer.
///     Supplied because C11 does not mandate fixed
///     width atomic support.
///
/// \typedef atomic_int64_t
/// \ingroup ym_core
/// \brief
///     Atomic signed 64-bit integer.
///     Supplied because C11 does not mandate fixed
///     width atomic support.
///////////////////////////////////////////////////////////
typedef atomic_schar  atomic_int8_t;
typedef atomic_short  atomic_int16_t;
typedef atomic_int    atomic_int32_t;
typedef atomic_long   atomic_int64_t;

///////////////////////////////////////////////////////////
/// \typedef atomic_uint8_t
/// \ingroup ym_core
/// \brief
///     Atomic unsigned 8-bit integer.
///     Supplied because C11 does not mandate fixed
///     width atomic support.
///
/// \typedef atomic_uint16_t
/// \ingroup ym_core
/// \brief
///     Atomic unsigned 16-bit integer.
///     Supplied because C11 does not mandate fixed
///     width atomic support.
///
/// \typedef atomic_uint32_t
/// \ingroup ym_core
/// \brief
///     Atomic unsigned 32-bit integer.
///     Supplied because C11 does not mandate fixed
///     width atomic support.
///
/// \typedef atomic_uint64_t
/// \ingroup ym_core
/// \brief
///     Atomic unsigned 64-bit integer.
///     Supplied because C11 does not mandate fixed
///     width atomic support.
///////////////////////////////////////////////////////////
typedef atomic_uchar  atomic_uint8_t;
typedef atomic_ushort atomic_uint16_t;
typedef atomic_uint   atomic_uint32_t;
typedef atomic_ulong  atomic_uint64_t;
