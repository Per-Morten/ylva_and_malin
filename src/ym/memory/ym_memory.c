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

ym_errc
ym_mem_init()
{
    static_assert(sizeof(ym_mem_region) * YM_MEM_REG_COUNT < YM_MEM_REG_GFX_OFFSET,
                  "Region heads will overflow into GFX region");

    ym_g_memory = malloc(YM_MEMORY_SIZE);

    memset(ym_g_memory, 0xFFFFFFFF, YM_MEMORY_SIZE);

    if (ym_g_memory == NULL)
    {
        YM_WARN("Could not allocate enough memory: %d",
                YM_MEMORY_SIZE);
        return ym_errc_bad_alloc;
    }


    int offsets[] =
    {
        YM_MEM_REG_REGION_HEADS_OFFSET,
        YM_MEM_REG_GFX_OFFSET,
        YM_MEM_REG_GL_OFFSET,
        YM_MEM_REG_TELEMETRY_OFFSET,
    };

    int sizes[] =
    {
        YM_MEM_REG_REGION_HEADS_BLOCK_SIZE,
        YM_MEM_REG_GFX_BLOCK_SIZE,
        YM_MEM_REG_GL_BLOCK_SIZE,
        YM_MEM_REG_TELEMETRY_BLOCK_SIZE,
    };

    YM_DEBUG("      Memory begin-end: %p %p",
             ym_g_memory,
             ym_g_memory + YM_MEM_REG_TELEMETRY_OFFSET + YM_MEM_REG_TELEMETRY_BLOCK_SIZE);

    for (int i = ym_mem_reg_region_heads; i < ym_mem_reg_count; i++)
    {
        ym_g_regions[i].id = i;
        ym_errc errc = ym_create_allocator(ym_alloc_strategy_region,
                            ym_g_memory + offsets[i],
                            sizes[i],
                            &(ym_allocator_cfg) // Temporary hack to get the game working. This should be something else.
                            {
                                .slot_size = (int[]){16, 32, 128, 1024},
                                .slot_count = (int[]){4, 4, 4, 1},
                                .count = 4,
                            },
                            &ym_g_regions[i]);

        if (errc != ym_errc_success)
            YM_WARN("Did not properly initialize: %s", ym_mem_reg_id_str(i));

        //YM_DEBUG("i: %d, Region begin-end: %p %p",
        //         i,
        //         ym_g_memory + offsets[i][0],
        //         ym_g_memory + offsets[i][0] + offsets[i][1]);

    }

    return ym_errc_success;
}

ym_errc
ym_mem_shutdown()
{
    #ifdef YM_MEMORY_TRACKING
    extern void ym_print_allocator_logs();
    ym_print_allocator_logs();
    #endif

    // Comment this back in after testing!
    free(ym_g_memory);

    return ym_errc_success;
}

void*
ym_mem_reg_alloc(ym_mem_reg_id id, int size, YM_UNUSED char* file, YM_UNUSED int line)
{
    void* ptr;
    ym_errc errc = YM_ALLOCATE(&ym_g_regions[id], size, &ptr);
    if (errc != ym_errc_success)
        YM_WARN("%s: memory allocation not without failure.", ym_errc_str(errc));
    return ptr;
}

void
ym_mem_reg_dealloc(ym_mem_reg_id id, int size, void* ptr, YM_UNUSED char* file, YM_UNUSED int line)
{
    ym_errc errc = YM_DEALLOCATE(&ym_g_regions[id], size, ptr);
    if (errc != ym_errc_success)
        YM_WARN("%s: memory deallocation not without failure.", ym_errc_str(errc));
}
