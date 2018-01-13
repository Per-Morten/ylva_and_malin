#include <ym_allocator.h>
#include <ym_memory_regions.h>


// TODO:
// Document and test this stuff.
// Not sure if I am proud or embarrassed for creating a linked list
// using only addresses, rather than creating a struct,
// But I don't think it would be pretty either way.
// But these stuff, they seriously need tests.
static
ym_errc
init_free_list_allocator(ym_allocator* allocator)
{
    // Setup free-list heads

    uintptr_t* heads = allocator->mem;
    allocator->used += 4 * sizeof(heads);

    // For testing, support:
    // Size - count
    // 16   - 4
    // 64   - 4
    // 256  - 4
    // 1024 - 4

    int slot_sizes[4] =
    {
        16, 32, 256, 1024,
    };

    int slots[4] =
    {
        //32, 16, 4, 2,
        // 30 instead of 32 as
        // 32 (2 * 16 byte slots) bytes are used for headers
        30, 16, 4, 2,
    };

    for (int i = 0; i < 4; i++)
    {
        heads[i] = allocator->mem + allocator->used;
        allocator->used += slots[i] * slot_sizes[i];
        //YM_DEBUG("Head: %p", heads[i]);
        uintptr_t* lstart = heads[i];
        for (int j = 0; j < slots[i] - 1; j++)
        {
            *lstart = (u8*)lstart + slot_sizes[i];
            //YM_DEBUG("Link: %p to %p", lstart, *lstart);
            lstart = *lstart;
        }
        *lstart = NULL;
        //YM_DEBUG("Link: %p to %p", lstart, *lstart);
    }

    if (allocator->id == 1)
    {
        for (int i = 0; i < 4; i++)
        {
            uintptr_t* start = heads[i];
            int count = 0;
            while (start)
            {
                YM_DEBUG("%d, %p, %p", count++, start, *start);
                start = *start;
            }
            YM_DEBUG("Used: %u", allocator->used);

        }
    }

    return ym_errc_success;
}

static
ym_errc
free_list_allocate(ym_allocator* allocator, int size, void** ptr)
{
    //YM_DEBUG("Allocating: %d from region: %s, %p",
    //         size,
    //         ym_mem_reg_id_str(allocator->id),
    //         *(uintptr_t*)allocator->mem);

    int pool = (size < 16) ? 0
             : (size < 32) ? 1
             : (size < 256) ? 2
             : 3;

    uintptr_t* heads = allocator->mem;
    if (heads[pool])
    {
        uintptr_t* mem = heads[pool];
        uintptr_t* next = *(uintptr_t*)heads[pool];
        (*ptr) = mem;
        //YM_DEBUG("Pool: %d, mem %p, Heads: %p, next: %p", pool, mem, heads[pool], next);
        heads[pool] = next;
    }
    else
    {
        //YM_DEBUG("Not enough room");
        (*ptr) = NULL;
        // Return error here!
    }



//    (*ptr) = malloc(size);
    return ym_errc_success;
}

static
ym_errc
free_list_deallocate(ym_allocator* allocator, int size, void* ptr)
{
    // TODO: Find out why seemingly it doesn't mather to the stack how I deallocate.
    // Its the same no matter which order I deallocate in.
    // Create test that deletes in an order I can use to
    // double check that is is happening correctly.

    // REMEMBER TO MEMSET MEMORY HERE! So it can be used to check for leaks etc.
    //YM_DEBUG("Deallocating: %d to region: %s, %p",
    //         size,
    //         ym_mem_reg_id_str(allocator->id),
    //         ptr);
    //free(ptr);

    if (!ptr)
    {
        //YM_DEBUG("Sending in NULL");
        return ym_errc_invalid_input;
    }

    int pool = (size < 16) ? 0
             : (size < 32) ? 1
             : (size < 256) ? 2
             : 3;

    uintptr_t* heads = allocator->mem;
    uintptr_t* slot = ptr;
    *slot = heads[pool];
    heads[pool] = slot;


   if (allocator->id == 1)
   {
       for (int i = 0; i < 4; i++)
       {
           uintptr_t* start = heads[i];
           int count = 0;
           while (start)// && count < 5)
           {
               //YM_DEBUG("%d, %p, %p", count++, start, *start);
               start = *start;
           }
           //YM_DEBUG("Used: %u", allocator->used);

       }
   }




    return ym_errc_success;
}

ym_errc
ym_create_allocator(ym_alloc_strategy strategy,
                    void* memory,
                    uint size,
                    ym_allocator_cfg* allocator_cfg,
                    ym_allocator* out_allocator)
{
    out_allocator->strategy = strategy;
    out_allocator->mem = memory;
    out_allocator->size = size;
    out_allocator->used = 0;

    ym_errc errc = ym_errc_success;
    if (strategy = ym_alloc_strategy_region)
        errc |= init_free_list_allocator(out_allocator);

    return errc;
}

ym_errc
ym_destroy_allocator(ym_allocator* allocator)
{
    return ym_errc_success;
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
ym_pool_allocate(YM_UNUSED ym_allocator* allocator,
                 YM_UNUSED int size,
                 YM_UNUSED void** ptr)
{
    return ym_errc_success;
}

static
ym_errc
ym_errc_linear_deallocate(YM_UNUSED ym_allocator* allocator,
                          YM_UNUSED int size,
                          YM_UNUSED void* ptr)
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

        case ym_alloc_strategy_region:
            //(*ptr) = malloc(size);
            //errc = (*ptr) ? ym_errc_success : ym_errc_bad_alloc;
            errc = free_list_allocate(allocator, size, ptr);
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
        //YM_WARN("%s: Requested allocation of %d to large, falling back to malloc",
        //        ym_errc_str(errc), size);
        // HACK!
        errc = ym_errc_success;
        //EO HACK!
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
        case ym_alloc_strategy_region:
            errc = free_list_deallocate(allocator, size, ptr);
        break;

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
        //YM_WARN("%s: Requested deallocation not within memory, speculatively freeing with free",
        //        ym_errc_str(errc));
        free(ptr);
        // HACK
        errc = ym_errc_success;
        // EO HACK!
    }
    else if (errc != ym_errc_success)
    {
        YM_WARN("%s: Invalid pointer given to ym_deallocate",
                ym_errc_str(errc));
    }

    return errc;
}
