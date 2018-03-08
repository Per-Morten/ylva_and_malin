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

// Pushing off this diagnostic because with the free-list it is a mess to read if I have to cast ALL the time.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-conversion"
static
ym_errc
init_free_list_allocator(ym_allocator* allocator, ym_allocator_cfg* cfg)
{
    if (!cfg)
    {
        // Use defaults here.
        YM_DEBUG("cfg is NULL");
    }

    const int count = cfg->count;
    u16* sizes = allocator->free_list.slot_size;
    u8* slots = allocator->free_list.slot_count;
    uintptr_t* heads = allocator->free_list.heads;

    memset(sizes, 0, sizeof(allocator->free_list.slot_size));
    memset(slots, 0, sizeof(allocator->free_list.slot_count));
    memset(heads, 0, sizeof(allocator->free_list.heads));

    for (int i = 0; i < count; i++)
    {
        sizes[i] = cfg->slot_size[i];
        slots[i] = cfg->slot_count[i];

        heads[i] = allocator->mem + allocator->used;
        allocator->used += slots[i] * sizes[i];
        uintptr_t* itr = heads[i];

        for (int j = 0; j < slots[i] - 1; j++)
        {
            *itr = itr + sizes[i];
            itr = *itr;
        }
        *itr = NULL;
    }

    return ym_errc_success;
}

static
ym_errc
free_list_allocate(ym_allocator* allocator, int size, void** ptr)
{
    u16* sizes = allocator->free_list.slot_size;
    uintptr_t* heads = allocator->free_list.heads;

    int pool = 0;
    while (size > sizes[pool])
        pool++;

    if (pool >= YM_ALLOCATOR_SLOT_REGION_COUNT)
    {
        YM_WARN("%s: Requested allocation larger than any slots, size: %d",
                ym_errc_str(ym_errc_invalid_input), size);

        (*ptr) = malloc(size);
        return ym_errc_invalid_input;
    }

    // Out of memory for particular slot
    if (!heads[pool])
    {
        YM_WARN("%s: No more slots of size %d could be found, falling back to malloc",
                ym_errc_str(ym_errc_out_of_memory), sizes[pool]);

        (*ptr) = malloc(size);
        return ym_errc_out_of_memory;
    }

    uintptr_t* mem = heads[pool];
    uintptr_t* next = *(uintptr_t*)heads[pool];
    (*ptr) = mem;
    heads[pool] = next;

    return ym_errc_success;
}

static
ym_errc
free_list_deallocate(ym_allocator* allocator, int size, void* ptr)
{
    if (!ptr)
    {
        YM_WARN("%s: Tried to deallocate NULL pointer",
                ym_errc_str(ym_errc_invalid_input));

        return ym_errc_invalid_input;
    }

    // Deal with case where ptr is not actually from allocator, but probably! i.e. needs to be freed
    if (ptr < allocator->mem || ptr > allocator->mem + allocator->size)
    {
        YM_WARN("%s: Ptr outside of memory, freeing with free, [%p, %p) vs %p",
                ym_errc_str(ym_errc_invalid_input),
                allocator->mem, allocator->mem + allocator->size, ptr);

        free(ptr);
        return ym_errc_invalid_input;
    }

    int pool = 0;
    while (size > allocator->free_list.slot_size[pool])
        pool++;

    if (pool >= YM_ALLOCATOR_SLOT_REGION_COUNT)
    {
        YM_WARN("%s: Requested deallocation larger than any slots, size: %d",
                ym_errc_str(ym_errc_invalid_input), size);

        free(ptr);
        return ym_errc_invalid_input;
    }

    uintptr_t* slot = ptr;
    *slot = allocator->free_list.heads[pool];
    allocator->free_list.heads[pool] = slot;

    return ym_errc_success;
}
#pragma GCC diagnostic pop

#ifdef YM_MEMORY_TRACKING
#define YM_MEMORY_TRACKING_MAX_SIZE 2048
#define YM_MEMORY_TRACKING_MAX_STR_SIZE 20

// This probably shouldn't be allocated here, it should be heap allocated in own debug memory region.
static struct
{
    int line;
    int size;
    char file[YM_MEMORY_TRACKING_MAX_STR_SIZE];
    const void* ptr;
} ym_memory_tracking_info[YM_MEMORY_TRACKING_MAX_SIZE];

static
void
track_allocation(YM_UNUSED ym_allocator* allocator,
                 int size,
                 void** ptr,
                 char* file,
                 int line)
{
    #ifdef WIN32
    const char* filename = strrchr(file, '\\');
    #else
    const char* filename = strrchr(file, '/');
    #endif
    filename++;

    for (int i = 0; i < YM_MEMORY_TRACKING_MAX_SIZE; i++)
    {
        if (ym_memory_tracking_info[i].line == 0)
        {
            ym_memory_tracking_info[i].line = line;
            ym_memory_tracking_info[i].size = size;
            strcpy(ym_memory_tracking_info[i].file, filename);
            ym_memory_tracking_info[i].ptr = (*ptr);
            return;
        }
    }

    YM_WARN("%s: Could not track allocation, YM_MEMORY_TRACKING_MAX_SIZE exceeded",
            ym_errc_str(ym_errc_out_of_memory));
}

static
void
track_deallocation(YM_UNUSED ym_allocator* allocator,
                   YM_UNUSED int size,
                   YM_UNUSED void* ptr,
                   char* file,
                   int line)
{
    #ifdef WIN32
    const char* filename = strrchr(file, '\\');
    #else
    const char* filename = strrchr(file, '/');
    #endif
    filename++;

    for (int i = 0; i < YM_MEMORY_TRACKING_MAX_SIZE; i++)
    {
        if (ym_memory_tracking_info[i].ptr == ptr)
        {
            ym_memory_tracking_info[i].line = 0;
            return;
        }
    }

    YM_WARN("%s: Could not find respective allocation to allocation in: %s, line: %d, %p",
            ym_errc_str(ym_errc_invalid_input),
            filename, line, ptr);
}

void
ym_print_allocator_logs()
{
    for (int i = 0; i < YM_MEMORY_TRACKING_MAX_SIZE; i++)
    {
        if (ym_memory_tracking_info[i].line != 0)
        {
            YM_WARN("%s: leak detected, file: %s, line: %d, size: %d",
                    ym_errc_str(ym_errc_mem_leak),
                    ym_memory_tracking_info[i].file,
                    ym_memory_tracking_info[i].line,
                    ym_memory_tracking_info[i].size);
        }
    }
}
#endif

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
    if (strategy == ym_alloc_strategy_region)
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
#ifdef YM_MEMORY_TRACKING
ym_allocate(ym_allocator* allocator, int size, void** ptr, char* file, int line)
#else
ym_allocate(ym_allocator* allocator, int size, void** ptr)
#endif
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

    #ifdef YM_MEMORY_TRACKING
    track_allocation(allocator, size, ptr, file, line);
    #endif

    return errc;
}

ym_errc
#ifdef YM_MEMORY_TRACKING
ym_deallocate(ym_allocator* allocator, int size, void* ptr, char* file, int line)
#else
ym_deallocate(ym_allocator* allocator, int size, void* ptr)
#endif
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

    #ifdef YM_MEMORY_TRACKING
    if (ptr)
        track_deallocation(allocator, size, ptr, file, line);
    #endif

    return errc;
}

