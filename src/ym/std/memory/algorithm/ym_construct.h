#pragma once
#include <utility>
#include <ym_macros.h>

namespace ym
{
    template <class T,
              class... Args>
    YM_INLINE
    T*
    construct(T* mem,
              Args&&... args)
    {
        YM_ASSERT(mem, "Trying to create on nullptr");
        return ::new (static_cast<void*>(mem)) T(std::forward<Args>(args)...);
    }

    template <class InputIt,
              class ForwardIt>
    YM_INLINE
    ForwardIt
    uninitialized_copy(InputIt begin,
                       InputIt end,
                       ForwardIt d_begin)
    {
        while (begin != end)
            ym::construct(std::addressof(*d_begin++),
                          typename std::iterator_traits<ForwardIt>::value_type(*begin++));
        return d_begin;
    }

    template <class InputIt,
              class ForwardIt>
    YM_INLINE
    ForwardIt
    uninitialized_move(InputIt begin,
                       InputIt end,
                       ForwardIt d_begin)
    {
        while (begin != end)
            ym::construct(std::addressof(*d_begin++),
                          typename std::iterator_traits<ForwardIt>::value_type(std::move(*begin++)));
        return d_begin;
    }

    template <class ForwardIt,
              class T>
    YM_INLINE
    void
    uninitialized_fill(ForwardIt begin,
                       ForwardIt end,
                       const T& value)
    {
        while (begin != end)
            ym::construct(std::addressof(*begin++), value);

    }
}
