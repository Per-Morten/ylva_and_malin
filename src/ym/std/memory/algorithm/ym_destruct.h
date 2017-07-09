#pragma once
#include <utility>
#include <memory>
#include <ym_macros.h>

namespace ym
{
    template <class T>
    YM_INLINE
    void
    destruct(T* object)
    {
        YM_ASSERT(object, "Trying to destruct nullptr");
        object->~T();
    }

    template <class InputItr>
    YM_INLINE
    void
    destruct(InputItr begin,
             const InputItr end)
    {
        while (begin != end)
            destruct(std::addressof(*begin++));
    }
}
