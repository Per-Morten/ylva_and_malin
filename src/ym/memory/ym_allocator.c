#include <ym_allocator.h>

/// \todo Turn region into allocators!

ym_errc
ym_create_allocator(ym_mem_region* region,
                    ym_alloc_strategy strategy,
                    uint size,
                    ym_allocator* out_allocator)
{
    if (region->used + size >= region->size)
    {
        YM_WARN("Allocating more than region can handle: %u vs %u",
                region->size + region->used, size);
        return;
    }

    out_allocator->strategy = strategy;
    out_allocator->mem = region->mem + region->used;
    out_allocator->size = size;
    out_allocator->used = 0;

    region->used += size;
}

ym_errc
ym_destroy_allocator(ym_mem_region* region,
                     ym_allocator* allocator)
{
    memset(allocator->mem, 0xFFFFFFFF, allocator->size);
    region->used -= allocator->size;
}

static
ym_errc
ym_stack_allocate(ym_allocator* allocator, int size, void** ptr)
{
    if (allocator->used + size >= allocator->size)
        return ym_errc_out_of_memory;

    (*ptr) = allocator->mem + size;
    allocator->used += size;

    return ym_errc_success;
}

static
ym_errc
ym_stack_deallocate(ym_allocator* allocator, int size, void* ptr)
{
    // Not deallocating in reverse order of allocation
    if (ptr != allocator->mem + allocator->used - size)
        return ym_errc_invalid_input;

    allocator->used -= size;

    return ym_errc_success;
}

static
ym_errc
ym_pool_allocate(ym_allocator* allocator, int size, void** ptr)
{
    return ym_errc_success;
}

static
ym_errc
ym_errc_linear_deallocate(ym_allocator* allocator, int size, void* ptr)
{
    YM_WARN("Trying to deallocate from linear allocator, clear and deallocate not implemented yet");
    return ym_errc_success;
}

ym_errc
ym_allocate(ym_allocator* allocator, int size, void** ptr)
{
    ym_errc errc;
    switch (allocator->strategy)
    {
        case ym_alloc_strategy_linear:
            // Fallthrough on purpose,
            // as allocating from stack and linear is same thing
        case ym_alloc_strategy_stack:
            errc = ym_stack_allocate(allocator, size, ptr);
        break;

        case ym_alloc_strategy_pool:
            errc = ym_pool_allocate(allocator, size, ptr);
        break;

        default:
            // Not really pretty
            errc = ym_errc_invalid_input;
            YM_WARN("%s: Unknown allocation strategy!",
                    ym_errc_str(errc));
            (*ptr) = NULL;
            return errc;
        break;
    }

    if (errc == ym_errc_success)
        return errc;

    if (errc == ym_errc_out_of_memory)
    {
        YM_WARN("%s: Requested allocation of %d to large, falling back to malloc",
                ym_errc_str(errc), size);
        (*ptr) = malloc(size);
    }
    else
    {
        YM_WARN("Allocation unsuccessful: %s", ym_errc_str(errc));
        (*ptr) = NULL;
    }

    return errc;
}

ym_errc
ym_deallocate(ym_allocator* allocator, int size, void* ptr)
{
    ym_errc errc;
    switch (allocator->strategy)
    {
        case ym_alloc_strategy_linear:
            errc = ym_errc_linear_deallocate(allocator, size, ptr);
        break;
        case ym_alloc_strategy_stack:
            errc = ym_stack_deallocate(allocator, size, ptr);
        break;

        case ym_alloc_strategy_pool:
            //errc = ym_pool_deallocate(allocator, size, ptr);
        break;

        default:
            errc = ym_errc_invalid_input;
            YM_WARN("%s: Unknown allocation strategy!",
                    ym_errc_str(errc));
            return errc;
        break;
    }

    #ifdef YM_SANITIZE_MEMORY
        memset(*ptr, 0xFFFFFFFF, size);
    #endif

    if (errc == ym_errc_invalid_input && ptr > allocator->mem + allocator->size)
    {
        // Checking if we can gamble that the allocation happened with malloc
        YM_WARN("%s: Requested deallocation not within memory, speculatively freeing with free",
                ym_errc_str(errc));
        free(ptr);
    }
    else
    {
        YM_WARN("%s: Invalid pointer given to ym_deallocate",
                ym_errc_str(errc));
    }

    return errc;
}
