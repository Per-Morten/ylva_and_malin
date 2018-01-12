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

    /// \todo This should be atomic once ym_atomic is in place.
    uint16_t used;
} ym_mem_region;

ym_errc
ym_mem_init();

ym_errc
ym_mem_shutdown();

ym_mem_region*
ym_mem_get_region(ym_mem_reg_id id);

void*
ym_mem_reg_alloc(ym_mem_reg_id id, int size, char* file, int line);

void
ym_mem_reg_dealloc(ym_mem_reg_id id, int size, void* ptr, char* file, int line);

// For simplicity, go for a linked list approach first
#define YM_MALLOC(reg_id, size) \
ym_mem_reg_alloc(reg_id, size, __FILE__, __LINE__);

#define YM_FREE(reg_id, size, ptr) \
ym_mem_reg_dealloc(reg_id, size, ptr, __FILE__, __LINE__);

