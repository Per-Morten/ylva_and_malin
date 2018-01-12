#pragma once
#include <ym_core.h>
#include <ym_memory.h>

typedef
enum
{
    ym_alloc_strategy_stack,
    ym_alloc_strategy_pool,
    ym_alloc_strategy_linear,

    // Add strategies for aligned allocations?
} ym_alloc_strategy;

typedef
struct
{
    void* mem;
    uint16_t size;
    uint16_t used;
    ym_alloc_strategy strategy;

    // Think of how to add extra variables that might be needed?
    // Maybe union?
} ym_allocator;

ym_errc
ym_create_allocator(ym_mem_region* region,
                    ym_alloc_strategy strategy,
                    uint size,
                    ym_allocator* out_allocator);

ym_errc
ym_destroy_allocator(ym_mem_region* region,
                     ym_allocator* allocator);

ym_errc
ym_allocate(ym_allocator* allocator, int size, void** ptr);

ym_errc
ym_deallocate(ym_allocator* allocator, int size, void* ptr);
