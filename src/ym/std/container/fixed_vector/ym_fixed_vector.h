#pragma once
#include <iterator>
#include <initializer_list>

#include <ym_memory.h>
#include <ym_types.h>

namespace ym
{
    template <class T,
              class Allocator = decltype(ym::memory_globals::default_allocator)>
    class fixed_vector
    {
    public:
        /////////////////////////////////////////////////////////////
        /// Member types
        /////////////////////////////////////////////////////////////
        using value_type = T;
        using size_type = ym::size_t;
        using difference_type = ym::ptrdiff_t;
        using reference = T&;
        using const_reference = const T&;
        using pointer = T*;
        using const_pointer = const T*;
        using iterator = T*;
        using const_iterator = const T*;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        /////////////////////////////////////////////////////////////
        /// Constructors / Destructors
        /////////////////////////////////////////////////////////////
        fixed_vector() = delete;

        fixed_vector(size_type cap,
                     Allocator* allocator = &ym::memory_globals::default_allocator) YM_NOEXCEPT;

        fixed_vector(size_type cap,
                     size_type count,
                     const_reference value,
                     Allocator* allocator = &ym::memory_globals::default_allocator) YM_NOEXCEPT;

        fixed_vector(const fixed_vector& src) YM_NOEXCEPT;

        fixed_vector(const fixed_vector& src,
                     Allocator* allocator) YM_NOEXCEPT;

        fixed_vector(fixed_vector&& src) YM_NOEXCEPT;

        fixed_vector(fixed_vector&& src,
                     Allocator* allocator) YM_NOEXCEPT;

        ~fixed_vector() YM_NOEXCEPT;

        /////////////////////////////////////////////////////////////
        /// Element Access
        /////////////////////////////////////////////////////////////
        YM_NO_DISCARD
        YM_INLINE
        reference
        operator[](size_type pos) YM_NOEXCEPT;

        YM_NO_DISCARD
        YM_INLINE
        const_reference
        operator[](size_type pos) const YM_NOEXCEPT;

        YM_NO_DISCARD
        YM_INLINE
        pointer
        data() YM_NOEXCEPT;

        YM_NO_DISCARD
        YM_INLINE
        const_pointer
        data() const YM_NOEXCEPT;

        /////////////////////////////////////////////////////////////
        /// Iterators
        /////////////////////////////////////////////////////////////
        YM_NO_DISCARD
        YM_INLINE
        iterator
        begin() YM_NOEXCEPT;

        YM_NO_DISCARD
        YM_INLINE
        const_iterator
        begin() const YM_NOEXCEPT;

        YM_NO_DISCARD
        YM_INLINE
        const_iterator
        cbegin() const YM_NOEXCEPT;

        YM_NO_DISCARD
        YM_INLINE
        iterator
        end() YM_NOEXCEPT;

        YM_NO_DISCARD
        YM_INLINE
        const_iterator
        end() const YM_NOEXCEPT;

        YM_NO_DISCARD
        YM_INLINE
        const_iterator
        cend() const YM_NOEXCEPT;

        /////////////////////////////////////////////////////////////
        /// Capacity
        /////////////////////////////////////////////////////////////
        YM_NO_DISCARD
        YM_INLINE
        bool
        empty() const YM_NOEXCEPT;

        YM_NO_DISCARD
        YM_INLINE
        size_type
        size() const YM_NOEXCEPT;

        YM_NO_DISCARD
        YM_INLINE
        size_type
        capacity() const YM_NOEXCEPT;

        /////////////////////////////////////////////////////////////
        /// Modifiers
        /////////////////////////////////////////////////////////////
        void
        clear() YM_NOEXCEPT;

        void
        push_back(const_reference value) YM_NOEXCEPT;

        void
        push_back(value_type&& value) YM_NOEXCEPT;

        void
        pop_back() YM_NOEXCEPT;

    private:
        Allocator* allocator_{};
        T* begin_{};
        T* end_{};
        T* cap_{};

    };
}

#include <ym_fixed_vector.tpp>
