#include <ym_memory.h>
#include <ym_log.h>
#include <stdlib.h>
#include <inttypes.h>
#include <ym_assert.h>
#include <ym_allocator.h>

//#define YM_MEMORY_DEFAULT_SIZE 8192
#define YM_MEMORY_DEFAULT_SIZE 16384

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
/// Can use this to check for leaks

static void* ym_g_memory;
static ym_allocator ym_g_regions[ym_mem_reg_count];

static u8 g_test_memory[YM_MEMORY_SIZE];


ym_errc
ym_mem_init()
{
    static_assert(sizeof(ym_mem_region) * YM_MEM_REG_COUNT < YM_MEM_REG_GFX_OFFSET,
                  "Region heads will overflow into GFX region");

    ym_g_memory = malloc(YM_MEMORY_SIZE);
    // Hack
    //ym_g_memory = g_test_memory;
    // Eo Hack

    memset(ym_g_memory, 0xFFFFFFFF, YM_MEMORY_SIZE);

    if (ym_g_memory == NULL)
    {
        YM_WARN("Could not allocate enough memory: %d",
                YM_MEMORY_SIZE);
        return ym_errc_bad_alloc;
    }


    int offsets[][2] =
    {
        { YM_MEM_REG_REGION_HEADS_OFFSET, YM_MEM_REG_REGION_HEADS_BLOCK_SIZE},
        { YM_MEM_REG_GFX_OFFSET, YM_MEM_REG_GFX_BLOCK_SIZE },
        { YM_MEM_REG_GL_OFFSET, YM_MEM_REG_GL_BLOCK_SIZE },
        { YM_MEM_REG_TELEMETRY_OFFSET, YM_MEM_REG_TELEMETRY_BLOCK_SIZE },
    };

    YM_DEBUG("      Memory begin-end: %p %p",
             ym_g_memory,
             ym_g_memory + YM_MEM_REG_TELEMETRY_OFFSET + YM_MEM_REG_TELEMETRY_BLOCK_SIZE);

    for (int i = ym_mem_reg_region_heads; i < ym_mem_reg_count; i++)
    {
        ym_g_regions[i].id = i;
        ym_errc errc = ym_create_allocator(ym_alloc_strategy_region,
                            ym_g_memory + offsets[i][0],
                            offsets[i][1],
                            &ym_g_regions[i]);

        if (errc != ym_errc_success)
            YM_WARN("Did not properly initialize: %s", ym_mem_reg_id_str(i));

        //YM_DEBUG("i: %d, Region begin-end: %p %p",
        //         i,
        //         ym_g_memory + offsets[i][0],
        //         ym_g_memory + offsets[i][0] + offsets[i][1]);

    }


    // This wastest memory, gfx should probably be first!
//    ym_g_regions[ym_mem_reg_gfx].mem = ym_g_memory + YM_MEM_REG_GFX_OFFSET;
//    ym_g_regions[ym_mem_reg_gfx].size = YM_MEM_REG_GFX_BLOCK_SIZE;
//    ym_g_regions[ym_mem_reg_gfx].used = 0;
//    ym_g_regions[ym_mem_reg_gfx].strategy = ym_alloc_strategy_region;
//
//    ym_g_regions[ym_mem_reg_gl].mem = ym_g_memory + YM_MEM_REG_GL_OFFSET;
//    ym_g_regions[ym_mem_reg_gl].size = YM_MEM_REG_GL_BLOCK_SIZE;
//    ym_g_regions[ym_mem_reg_gl].used = 0;
//    ym_g_regions[ym_mem_reg_gl].strategy = ym_alloc_strategy_region;
//
//    ym_g_regions[ym_mem_reg_telemetry].mem = ym_g_memory + YM_MEM_REG_TELEMETRY_OFFSET;
//    ym_g_regions[ym_mem_reg_telemetry].size = YM_MEM_REG_TELEMETRY_BLOCK_SIZE;
//    ym_g_regions[ym_mem_reg_telemetry].used = 0;
//    ym_g_regions[ym_mem_reg_telemetry].strategy = ym_alloc_strategy_region;


    // ym_mem_region* regions = ym_g_memory;
    // regions[ym_mem_reg_region_heads].id = YM_MEM_REG_REGION_HEADS_ID;
    // regions[ym_mem_reg_region_heads].mem = NULL;
    // regions[ym_mem_reg_region_heads].used = ATOMIC_VAR_INIT(0);
    // (*(u16*)&regions[ym_mem_reg_region_heads].size) = 0;

    // regions[ym_mem_reg_gfx].id = YM_MEM_REG_GFX_ID;
    // regions[ym_mem_reg_gfx].mem = (u8*)ym_g_memory + YM_MEM_REG_GFX_OFFSET;
    // regions[ym_mem_reg_gfx].used = ATOMIC_VAR_INIT(0);
    // (*(u16*)&regions[ym_mem_reg_gfx].size) = YM_MEM_REG_GFX_BLOCK_SIZE;

    // regions[ym_mem_reg_gl].id = YM_MEM_REG_GL_ID;
    // regions[ym_mem_reg_gl].mem = (u8*)ym_g_memory + YM_MEM_REG_GL_OFFSET;
    // regions[ym_mem_reg_gl].used = ATOMIC_VAR_INIT(0);
    // (*(u16*)&regions[ym_mem_reg_gl].size) = YM_MEM_REG_GL_BLOCK_SIZE;

    // regions[ym_mem_reg_telemetry].id = YM_MEM_REG_TELEMETRY_ID;
    // regions[ym_mem_reg_telemetry].mem = (u8*)ym_g_memory + YM_MEM_REG_TELEMETRY_OFFSET;
    // regions[ym_mem_reg_telemetry].used = ATOMIC_VAR_INIT(0);
    // (*(u16*)&regions[ym_mem_reg_gfx].size) = YM_MEM_REG_TELEMETRY_BLOCK_SIZE;

    //for (int i = 0; i != ym_mem_reg_count; i++)
    //    YM_DEBUG("%p", regions[i].mem);

    return ym_errc_success;
}

ym_errc
ym_mem_shutdown()
{
    // YM_UNUSED const ym_mem_region* region = ym_g_memory;
    // for (size_t i = ym_mem_reg_region_heads + 1; i < ym_mem_reg_count; ++i)
    // {
    //     YM_ASSERT(region[i].used == 0,
    //               ym_errc_mem_leak,
    //               "Leak detected in: %s, leak size: %" PRIu16 "",
    //               ym_mem_reg_id_str(region[i].id),
    //               region[i].used);
    // }

    // u32* mem = ym_g_memory + YM_MEM_REG_GFX_OFFSET;
    // const u32* memory_end = ym_g_memory + YM_MEM_REG_TELEMETRY_OFFSET + YM_MEM_REG_TELEMETRY_BLOCK_SIZE;

    // while (mem <= memory_end)
    // {
    //     if (*mem != 0xFFFFFFFF)
    //     {
    //         ym_mem_reg_id reg_id = 0;

    //         // Find leak region
    //         while (mem >= ((ym_mem_region*)ym_g_memory)[reg_id + 1].mem)
    //             reg_id++;

    //         //YM_WARN("Leak detected in region: %s, address: %p",
    //         //        ym_mem_reg_id_str(reg_id),
    //         //        mem);
    //     }
    //     mem++;
    // }

    // Comment this back in after testing!
    free(ym_g_memory);

    return ym_errc_success;
}

ym_mem_region*
ym_mem_get_region(ym_mem_reg_id region)
{
//    YM_ASSERT(region < ym_mem_reg_count && region != ym_mem_reg_region_heads,
//              ym_errc_invalid_input,
//              "Trying to get illegal region, %s",
//              ym_mem_reg_id_str(region));
//
//    YM_ASSERT(((ym_mem_region*)ym_g_memory)[region].id == region,
//              ym_errc_uninitialized,
//              "Region is not initialized: %s",
//              ym_mem_reg_id_str(region));
//
    return &((ym_mem_region*)ym_g_memory)[region];
}

void*
ym_mem_reg_alloc(ym_mem_reg_id id, int size, YM_UNUSED char* file, YM_UNUSED int line)
{
    void* ptr;
    ym_errc errc = ym_allocate(&ym_g_regions[id], size, &ptr);
    if (errc != ym_errc_success)
        YM_WARN("%s: memory allocation not without failure.", ym_errc_str(errc));
    return ptr;
}

void
ym_mem_reg_dealloc(ym_mem_reg_id id, int size, void* ptr, YM_UNUSED char* file, YM_UNUSED int line)
{
    ym_errc errc = ym_deallocate(&ym_g_regions[id], size, ptr);
    if (errc != ym_errc_success)
        YM_WARN("%s: memory deallocation not without failure.", ym_errc_str(errc));
}
