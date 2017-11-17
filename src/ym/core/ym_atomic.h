#pragma once
#include <ym_core.h>
#include <ym_types.h>
#include <ym_attributes.h>

// Add documentation

#ifdef WIN32
#include <windows.h>

// https://msdn.microsoft.com/en-us/library/windows/desktop/ms684122(v=vs.85).aspx
// According to https://en.wikipedia.org/wiki/Two%27s_complement
// I can also do unsigned with two's complement, meaning I can easily create
// unsigned atomic values.
typedef volatile LONG ym_atomic_i32 YM_ALIGNED(8);

YM_INLINE
i32
ym_atomic_i32_load(ym_atomic_i32* atm)
{
    i32 out = *atm;
    _ReadWriteBarrier();
    return out;
}

YM_INLINE
void
ym_atomic_i32_store(ym_atomic_i32* atm,
                    i32 val)
{
    *atm = val;
    _ReadWriteBarrier();
}

YM_INLINE
i32
ym_atomic_i32_add(ym_atomic_i32* atm,
                  i32 val)
{
    i32 out = InterlockedAddNoFence(atm, val);
    _ReadWriteBarrier();
    return out;
}

YM_INLINE
i32
ym_atomic_i32_sub(ym_atomic_i32* atm,
                  i32 val)
{
    i32 out = InterlockedAddNoFence(atm, val);
    _ReadWriteBarrier();
    return out;
}

YM_INLINE
i32
ym_atomic_i32_exchange(ym_atomic_i32* atm,
                       i32 val)
{
    i32 out = InterlockedExchangeNoFence(atm, val);
    _ReadWriteBarrier();
    return out;
}

YM_INLINE
bool
ym_atomic_i32_compare_exchange(ym_atomic_i32* atm,
                               i32* expected,
                               i32  desired)
{
    i32 actual = InterlockedCompareExchangeNoFence(atm,
                                                   desired,
                                                   *expected);
    _ReadWriteBarrier();
    return actual == *expected;
}

#else // Not WIN32

// Just do std atomics?

#endif

