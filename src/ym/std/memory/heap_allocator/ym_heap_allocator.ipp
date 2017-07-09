#include <cstdlib>

#if defined(YM_ASSERT_THROW) || defined(YM_ASSERT_TERMINATE)
YM_PUSH_WARNING_TERMINATE
YM_INLINE
ym::heap_allocator::~heap_allocator()
{
    YM_ASSERT(size_ == 0, "Not everything has been deleted");
}
YM_POP_WARNING
#endif

void*
ym::heap_allocator::allocate(ym::size_t size)
{
    size_ += size;
    return std::malloc(size);
}

void
ym::heap_allocator::deallocate(void* ptr,
                               ym::size_t size)
{
    if (ptr)
    {
        size_ -= size;
        std::free(ptr);
    }
}

void*
ym::heap_allocator::allocate_aligned(ym::size_t size,
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

    aligned_memory[-1] = static_cast<ym::byte>(adjustment);

    return static_cast<void*>(aligned_memory);
}

void
ym::heap_allocator::deallocate_aligned(void* ptr,
                                       ym::size_t size,
                                       ym::size_t alignment)
{
    if (ptr)
    {
        const auto aligned_memory
            = reinterpret_cast<ym::byte*>(ptr);

        const auto aligned_address
            = reinterpret_cast<ym::uptr_t>(ptr);

        const auto adjustment
            = static_cast<ym::ptrdiff_t>(aligned_memory[-1]);

        const ym::uptr_t raw_address
            = aligned_address - adjustment;

        deallocate(reinterpret_cast<void*>(raw_address), size + alignment);
    }
}

ym::size_t
ym::heap_allocator::size() const YM_NOEXCEPT
{
    return size_;
}
