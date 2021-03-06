#pragma once
#include <ym_core.h>

typedef
enum
{
    ym_alloc_strategy_stack,
    ym_alloc_strategy_pool,
    ym_alloc_strategy_linear,
    ym_alloc_strategy_region, // Make this buddy allocator in future, currently its fixed list

    // Add strategies for aligned allocations?
} ym_alloc_strategy;

// Used to send extra information about allocations.
typedef
struct
{
    // Only if region
    // Make into union if other members are added
    int* slot_size; // Must be sorted Ascending
    int* slot_count; // Number of blocks per slot
    int count; // Size of both arrays
} ym_allocator_cfg;

#define YM_ALLOCATOR_SLOT_REGION_COUNT 4

typedef
struct
{
    // For debug, remove when done
    int id;
    // Eo for debug


    void* mem;
    u16 size;
    u16 used;
    ym_alloc_strategy strategy;

    // These variables should be "private", i.e. not used in actual program
    // But are needed publicly for testing, among other things.
    union
    {
        struct
        {
            u16 slot_size[YM_ALLOCATOR_SLOT_REGION_COUNT];
            u8 slot_count[YM_ALLOCATOR_SLOT_REGION_COUNT];
            uintptr_t heads[YM_ALLOCATOR_SLOT_REGION_COUNT];
        } free_list;
    };

    // Think of how to add extra variables that might be needed?
    // Maybe union?
} ym_allocator;


ym_errc
ym_create_allocator(ym_alloc_strategy strategy,
                    void* memory,
                    uint size,
                    ym_allocator_cfg* allocator_cfg, // Can be NULL, if N/A, or to use defaults
                    ym_allocator* out_allocator);

ym_errc
ym_destroy_allocator(ym_allocator* allocator);

#define YM_MEMORY_TRACKING

#ifdef YM_MEMORY_TRACKING
ym_errc
ym_allocate(ym_allocator* allocator, int size, void** ptr, char* file, int line);
#else
ym_errc
ym_allocate(ym_allocator* allocator, int size, void** ptr);
#endif

#ifdef YM_MEMORY_TRACKING
ym_errc
ym_deallocate(ym_allocator* allocator, int size, void* ptr, char* file, int line);
#else
ym_errc
ym_deallocate(ym_allocator* allocator, int size, void* ptr);
#endif

#ifdef YM_MEMORY_TRACKING
#define YM_ALLOCATE(allocator, size, ptr) \
    ym_allocate(allocator, size, ptr, __FILE__, __LINE__);
#else
#define YM_ALLOCATE(allocator, size, ptr) \
    ym_allocate(allocator, size, ptr);
#endif

#ifdef YM_MEMORY_TRACKING
#define YM_DEALLOCATE(allocator, size, ptr) \
    ym_deallocate(allocator, size, ptr, __FILE__, __LINE__);
#else
#define YM_DEALLOCATE(allocator, size, ptr) \
    ym_deallocate(allocator, size, ptr);
#endif
