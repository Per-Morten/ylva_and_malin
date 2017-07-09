#pragma once
#include <ym_macros.h>
#include <ym_types.h>

namespace ym
{
    /////////////////////////////////////////////////////////////////
    /// \brief Class for doing general memory management on the
    ///        heap.
    ///        Supports both aligned an non aligned allocation.
    /////////////////////////////////////////////////////////////////
    class heap_allocator
    {
    public:
        /////////////////////////////////////////////////////////////
        /// \brief Destructor, ensures that all memory has been
        ///        deallocated.
        ///        Will not exist if asserts are not turned on.
        /////////////////////////////////////////////////////////////
        #if defined(YM_ASSERT_THROW) || defined(YM_ASSERT_TERMINATE)
        YM_INLINE
        ~heap_allocator();
        #endif

        /////////////////////////////////////////////////////////////
        /// \brief Allocates size bytes from the heap.
        ///
        /// \param size size to allocate in bytes.
        ///
        /// \return pointer to newly allocated memory, needs to be
        ///         deallocated with deallocate.
        /////////////////////////////////////////////////////////////
        YM_NO_DISCARD
        YM_INLINE
        void*
        allocate(ym::size_t size);

        /////////////////////////////////////////////////////////////
        /// \brief Releases the memory pointed to by ptr.
        ///
        /// \param ptr pointer pointing to memory to deallocate.
        /// \param size the size of the memory to deallocate.
        ///
        /// \note The whole pointer is deallocated, regardless of
        ///       what the size it.
        ///       It is only there for bookkeeping.
        ///       However it still needs to be the same as the size
        ///       that was used for allocation.
        /////////////////////////////////////////////////////////////
        YM_INLINE
        void
        deallocate(void* ptr,
                   ym::size_t size);

        /////////////////////////////////////////////////////////////
        /// \brief Allocates size bytes from the heap,
        ///        aligned to the desired alignment.
        ///
        /// \param size size to allocate in bytes.
        /// \param alignment desired alignment of the pointer.
        ///
        /// \return pointer to newly allocated memory, needs to be
        ///         deallocated with deallocate_aligned.
        /////////////////////////////////////////////////////////////
        YM_NO_DISCARD
        YM_INLINE
        void*
        allocate_aligned(ym::size_t size,
                         ym::size_t alignment);

        /////////////////////////////////////////////////////////////
        /// \brief Releases the memory pointed to by ptr.
        ///
        /// \param ptr pointer pointing to memory to deallocate.
        /// \param size the size of the memory to deallocate.
        /// \param alignment the aligment of the memory that was
        ///        allocated.
        ///
        /// \note The whole pointer is deallocated, regardless of
        ///       what the size it.
        ///       It is only there for bookkeeping.
        ///       However it still needs to be the same as the size
        ///       that was used for allocation.
        /////////////////////////////////////////////////////////////
        YM_INLINE
        void
        deallocate_aligned(void* ptr,
                           ym::size_t size,
                           ym::size_t alignment);

        /////////////////////////////////////////////////////////////
        /// \brief Returns the number of bytes that has been
        ///        allocated with this allocator.
        ///
        /// \return Total number of bytes allocated with the
        ///         allocator.
        /////////////////////////////////////////////////////////////
        YM_NO_DISCARD
        YM_INLINE
        ym::size_t
        size() const YM_NOEXCEPT;

    private:
        ym::size_t size_{0};
    };
}

#include <ym_heap_allocator.ipp>
