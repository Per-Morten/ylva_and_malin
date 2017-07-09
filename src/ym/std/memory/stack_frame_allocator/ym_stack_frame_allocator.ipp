#include <alloca.h>

#if defined(YM_ASSERT_THROW) || defined(YM_ASSERT_TERMINATE)
YM_PUSH_WARNING_TERMINATE
YM_INLINE
ym::stack_frame_allocator::~stack_frame_allocator()
{
    YM_ASSERT(size_ == 0, "Not everything has been deleted");
}
YM_POP_WARNING
#endif

void*
ym::stack_frame_allocator::allocate(ym::size_t size)
{
    size_ += size;
    return alloca(size);
}

void
ym::stack_frame_allocator::deallocate(YM_UNUSED void* ptr,
                                      ym::size_t size)
{
    if (ptr)
        size_ -= size;
}

void*
ym::stack_frame_allocator::allocate_aligned(ym::size_t size,
                                            ym::size_t alignment)
{
    YM_ASSERT(alignment >= 1, "Alignment needs to be >= 1");
    YM_ASSERT(alignment <= 128, "Alignment needs to be <= 128");
    YM_ASSERT((alignment & (alignment - 1)) == 0, "Alignment must be power of 2");

    const ym::size_t total = size + alignment;

    const auto address
        = reinterpret_cast<ym::uptr_t>(allocate(total));

    const auto mask = (alignment - 1);
    const ym::uptr_t misalignment = (address & mask);
    const ym::ptrdiff_t adjustment = alignment - misalignment;

    const ym::uptr_t aligned_address
        = address + adjustment;

    YM_ASSERT(adjustment < 256, "Adjustment to large");

    auto aligned_memory
        = reinterpret_cast<ym::byte*>(aligned_address);

    return static_cast<void*>(aligned_memory);
}

void
ym::stack_frame_allocator::deallocate_aligned(void* ptr,
                                              ym::size_t size,
                                              ym::size_t alignment)
{
    deallocate(ptr, size + alignment);
}

ym::size_t
ym::stack_frame_allocator::size() const YM_NOEXCEPT
{
    return size_;
}
