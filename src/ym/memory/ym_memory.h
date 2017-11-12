#pragma once
#include <stdint.h>
#include <stdatomic.h>

#include <ym_core.h>
#include <ym_memory_regions.h>

typedef
struct
{
    void* mem;
    ym_mem_reg_id id;
    const uint16_t size;

    // TODO: This should be atomic once ym_atomic is in place.
    uint16_t used;
} ym_mem_region;

ym_errc
ym_mem_init();

ym_errc
ym_mem_shutdown();

ym_mem_region*
ym_mem_get_region(ym_mem_reg_id id);

