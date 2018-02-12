#include <ym_allocator.h>
#include <ym_memory_regions.h>

// TODO:
// Document and test this stuff.
// Not sure if I am proud or embarrassed for creating a linked list
// using only addresses, rather than creating a struct,
// But I don't think it would be pretty either way.
// But these stuff, they seriously need tests.
//
// Thought: Is it possible to start claiming more areas in the case
//          of running out of one slot size? I.e. Take an existing slot that
//          is larger, and make it into smaller slots, sort of like a buddy allocation scheme.
static
ym_errc
init_free_list_allocator(ym_allocator* allocator, ym_allocator_cfg* cfg)
{
    // Setup free-list heads
    //YM_DEBUG("%d", cfg->count);

    if (!cfg)
    {
        YM_DEBUG("cfg is NULL");
    }

    const int count = cfg->count;
    u16* sizes = allocator->free_list.slot_size;
    u8* slots = allocator->free_list.slot_count;
    uintptr_t* heads = allocator->free_list.heads;

    memset(sizes, 0, sizeof(allocator->free_list.slot_size));
    memset(slots, 0, sizeof(allocator->free_list.slot_count));
    memset(heads, 0, sizeof(allocator->free_list.heads));

    //YM_DEBUG("Setup slots");
    for (int i = 0; i < count; i++)
    {
        sizes[i] = cfg->slot_size[i];
        slots[i] = cfg->slot_count[i];
        //YM_DEBUG("slots: %d, sizes: %d",(int)slots[i], (int)sizes[i]);
    }

    //YM_DEBUG("Create list");
    for (int i = 0; i < count; i++)
    {
        //YM_DEBUG("i: %d",i);
        heads[i] = allocator->mem + allocator->used;
        allocator->used += slots[i] * sizes[i];
        uintptr_t* itr = heads[i];
        //YM_DEBUG("used: %d", (int)allocator->used);
        for (int j = 0; j < slots[i] - 1; j++)
        {
            //YM_DEBUG("j: %d", j);
            //YM_DEBUG("%d", slots[i]);
            *itr = itr + sizes[i];
            itr = *itr;
        }
        *itr = NULL;
    }
    //YM_DEBUG("Finished with list");


    // uintptr_t* heads = allocator->mem;
    // allocator->used += 4 * sizeof(heads);

    // For testing, support:
    // Size - count
    // 16   - 4
    // 64   - 4
    // 256  - 4
    // 1024 - 4

//   int slot_sizes[4] =
//   {
//       16, 32, 256, 1024,
//   };
//
//   int slots[4] =
//   {
//       //32, 16, 4, 2,
//       // 30 instead of 32 as
//       // 32 (2 * 16 byte slots) bytes are used for headers
//       30, 16, 4, 2,
//   };
//
//   for (int i = 0; i < 4; i++)
//   {
//       heads[i] = allocator->mem + allocator->used;
//       allocator->used += slots[i] * slot_sizes[i];
//       //YM_DEBUG("Head: %p", heads[i]);
//       uintptr_t* lstart = heads[i];
//       for (int j = 0; j < slots[i] - 1; j++)
//       {
//           *lstart = (u8*)lstart + slot_sizes[i];
//           //YM_DEBUG("Link: %p to %p", lstart, *lstart);
//           lstart = *lstart;
//       }
//       *lstart = NULL;
//       //YM_DEBUG("Link: %p to %p", lstart, *lstart);
//   }
//
//   if (allocator->id == 1)
//   {
//       for (int i = 0; i < 4; i++)
//       {
//           uintptr_t* start = heads[i];
//           int count = 0;
//           while (start)
//           {
//               YM_DEBUG("%.8d, %.16p, %p", count++, start, *start);
//               start = *start;
//           }
//           YM_DEBUG("Used: %u", allocator->used);
//
//       }
//   }

    return ym_errc_success;
}

static
ym_errc
free_list_allocate(ym_allocator* allocator, int size, void** ptr)
{
//   YM_DEBUG("Allocating: %d from region: %s, %p",
//            size,
//            ym_mem_reg_id_str(allocator->id),
//            *(uintptr_t*)allocator->mem);
//
//    YM_DEBUG("Inside free_list_allocate");
//    int pool = (size < 16) ? 0
//             : (size < 32) ? 1
//             : (size < 256) ? 2
//             : 3;

    u16* sizes = allocator->free_list.slot_size;
    uintptr_t* heads = allocator->free_list.heads;

    int pool = 0;
    while (size > sizes[pool])
        pool++;

    if (pool >= YM_ALLOCATOR_SLOT_REGION_COUNT)
    {
        YM_WARN("%s: Requested allocation larger than any slots, size: %d",
                ym_errc_str(ym_errc_invalid_input), size);
        return ym_errc_invalid_input;
    }

    if (heads[pool])
    {
        //YM_DEBUG("Allocating from pool %d", pool);
        uintptr_t* mem = heads[pool];
        uintptr_t* next = *(uintptr_t*)heads[pool];
        (*ptr) = mem;
        //YM_DEBUG("Pool: %d, mem %p, Heads: %p, next: %p", pool, mem, heads[pool], next);
        heads[pool] = next;
    }
    else
    {
        YM_DEBUG("Couldn't find pool");
    }

//    uintptr_t* heads = allocator->mem;
//    if (heads[pool])
//    {
//        uintptr_t* mem = heads[pool];
//        uintptr_t* next = *(uintptr_t*)heads[pool];
//        (*ptr) = mem;
//        //YM_DEBUG("Pool: %d, mem %p, Heads: %p, next: %p", pool, mem, heads[pool], next);
//        heads[pool] = next;
//    }
//    else
//    {
//        //YM_DEBUG("Not enough room");
//        (*ptr) = NULL;
//        // Return error here!
//    }



//    (*ptr) = malloc(size);
    return ym_errc_success;
}

static
ym_errc
free_list_deallocate(ym_allocator* allocator, int size, void* ptr)
{
    // REMEMBER TO MEMSET MEMORY HERE! So it can be used to check for leaks etc.
    //YM_DEBUG("Deallocating: %d to region: %s, %p",
    //         size,
    //         ym_mem_reg_id_str(allocator->id),
    //         ptr);
    //free(ptr);

    if (!ptr)
    {
        YM_WARN("%s: Tried to deallocate NULL pointer",
                ym_errc_str(ym_errc_invalid_input));
        return ym_errc_invalid_input;
    }

    int pool = 0;
    while (size > allocator->free_list.slot_size[pool])
        pool++;

    uintptr_t* slot = ptr;
    *slot = allocator->free_list.heads[pool];
    allocator->free_list.heads[pool] = slot;


//    if (!ptr)
//    {
//        //YM_DEBUG("Sending in NULL");
//        return ym_errc_invalid_input;
//    }
//
//    int pool = (size < 16) ? 0
//             : (size < 32) ? 1
//             : (size < 256) ? 2
//             : 3;
//
//    uintptr_t* heads = allocator->mem;
//    uintptr_t* slot = ptr;
//    *slot = heads[pool];
//    heads[pool] = slot;
//
//
//   if (allocator->id == 1)
//   {
//       for (int i = 0; i < 4; i++)
//       {
//           uintptr_t* start = heads[i];
//           int count = 0;
//           while (start)// && count < 5)
//           {
//               //YM_DEBUG("%d, %p, %p", count++, start, *start);
//               start = *start;
//           }
//           //YM_DEBUG("Used: %u", allocator->used);
//
//       }
//   }
//
//
//
//
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
        errc |= init_free_list_allocator(out_allocator, allocator_cfg);

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

    YM_DEBUG("Outside");

    if (errc == ym_errc_success)
        return errc;

    if (errc == ym_errc_out_of_memory)
    {
        //YM_WARN("%s: Requested allocation of %d to large, falling back to malloc",
        //        ym_errc_str(errc), size);
        // HACK!
        YM_DEBUG("In out of memory");
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
