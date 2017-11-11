#include <ym_memory.h>
#include <ym_log.h>
#include <stdlib.h>
#include <inttypes.h>
#include <ym_assert.h>

#define YM_MEMORY_DEFAULT_SIZE 4096

#ifndef YM_MEMORY_SIZE
#pragma message("YM_MEMORY_SIZE not defined, using YM_MEMORY_DEFAULT_SIZE")
#define YM_MEMORY_SIZE YM_MEMORY_DEFAULT_SIZE
#endif

/// Create a large contiguous block of memory.
/// The first bytes will be used for storing the header blocks,
/// which points to the different memory regions.
///
/// Each subsystem is given their own memory region.
/// Supply different methods for allocating from the memory
/// in the regions.
/// For example stack allocator etc.
///
/// Keeping the header blocks allows us to check for leaks
/// when shutting down.
/// Having the regions in hand also allows us to inspect
/// how much memory that has been allocated etc.
///
/// When deallocating from regions, set memory value
/// to a detectable value. For example 0xFFFFFFFF

static void* ym_g_memory;

ym_errc
ym_mem_init()
{
    static_assert(sizeof(ym_mem_region) * YM_MEM_REG_COUNT < YM_MEM_REG_GFX_OFFSET,
                  "Region heads will overflow into GFX region");

    ym_g_memory = malloc(YM_MEMORY_SIZE);

    if (ym_g_memory == NULL)
    {
        YM_WARN("Could not allocate enough memory: %d",
                YM_MEMORY_SIZE);
        return ym_errc_bad_alloc;
    }

    ym_mem_region* regions = ym_g_memory;

    regions[ym_mem_reg_region_heads].id = YM_MEM_REG_REGION_HEADS_ID;
    regions[ym_mem_reg_region_heads].mem = NULL;
    regions[ym_mem_reg_region_heads].used = ATOMIC_VAR_INIT(0);
    (*(u16*)&regions[ym_mem_reg_region_heads].size) = 0;

    regions[ym_mem_reg_gfx].id = YM_MEM_REG_GFX_ID;
    regions[ym_mem_reg_gfx].mem = (u8*)ym_g_memory + YM_MEM_REG_GFX_OFFSET;
    regions[ym_mem_reg_gfx].used = ATOMIC_VAR_INIT(0);
    (*(u16*)&regions[ym_mem_reg_gfx].size) = YM_MEM_REG_GFX_BLOCK_SIZE;

    regions[ym_mem_reg_telemetry].id = YM_MEM_REG_TELEMETRY_ID;
    regions[ym_mem_reg_telemetry].mem = (u8*)ym_g_memory + YM_MEM_REG_TELEMETRY_OFFSET;
    regions[ym_mem_reg_telemetry].used = ATOMIC_VAR_INIT(0);
    (*(u16*)&regions[ym_mem_reg_gfx].size) = YM_MEM_REG_TELEMETRY_BLOCK_SIZE;

    return ym_errc_success;
}

ym_errc
ym_mem_shutdown()
{
    YM_UNUSED const ym_mem_region* region = ym_g_memory;
    for (size_t i = ym_mem_reg_region_heads + 1; i < ym_mem_reg_count; ++i)
    {
        YM_ASSERT(region[i].used == 0,
                  ym_errc_mem_leak,
                  "Leak detected in: %s, leak size: %" PRIu16 "",
                  ym_mem_reg_id_str(region[i].id),
                  region[i].used);
    }

    free(ym_g_memory);

    return ym_errc_success;
}

ym_mem_region*
ym_mem_get_region(ym_mem_reg_id region)
{
    YM_ASSERT(region < ym_mem_reg_count && region != ym_mem_reg_region_heads,
              ym_errc_invalid_input,
              "Trying to get illegal region, %s",
              ym_mem_reg_id_str(region));

    YM_ASSERT(((ym_mem_region*)ym_g_memory)[region].id == region,
              ym_errc_uninitialized,
              "Region is not initialized: %s",
              ym_mem_reg_id_str(region));

    return &((ym_mem_region*)ym_g_memory)[region];
}
