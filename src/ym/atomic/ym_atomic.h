#pragma once
#include <ym_core.h>

// Only support signed integers
// because windows does not have an API for unsigned integers. -.-...
// Think of what to do with this.

// Also, only support acquire release semantics.
// Need memory fences probably.

#ifdef WIN32
#include <windows.h>

// THIS ALSO NEEDS TO BE ALIGNED!!!!!
typedef volatile LONG     ym_atomic_i32;

YM_INLINE
i32
ym_atomic_i32_add(ym_atomic_i32* atm,
                  i32 val)
{
    return InterlockedAddRelease(atm, val);
}

YM_INLINE
i32
ym_atomic_i32_sub(ym_atomic_i32* atm,
                  i32 val)
{
    return InterlockedDecrementRelease(atm, val);
}

YM_INLINE
i32
ym_atomic_i32_load(ym_atomic_i32* atm)
{
    return InterlockedAddAcquire(atm, 0);
}

YM_INLINE
bool
ym_atomic_i32_cas(ym_atomic_i32* atm,
                  )
#else

#endif

