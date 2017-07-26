#pragma once
#include <stdint.h>

#include <ym_core.h>
#include <ym_memory_regions.h>

YM_EXTERN_CPP_BEGIN

typedef
struct
{
    void* mem;
    ym_mem_reg_id region;
    uint16_t size;
    uint16_t used;
} ym_mem_region;

ym_errc
ym_mem_init();

ym_errc
ym_mem_shutdown();

ym_mem_region*
ym_mem_create_region(ym_mem_reg_id id,
                     uint16_t size);

void
ym_mem_destroy_region(ym_mem_region* id);

YM_EXTERN_CPP_END
