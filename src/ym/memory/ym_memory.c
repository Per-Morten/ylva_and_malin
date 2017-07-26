#include <ym_memory.h>
#include <ym_log.h>
#include <stdlib.h>
#include <inttypes.h>

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

static void* ym_g_memory;

ym_errc
ym_mem_init()
{
    ym_g_memory = malloc(YM_MEMORY_SIZE);
    return ym_g_memory != NULL
         ? ym_errc_success
         : ym_errc_bad_alloc;
}

// Go through all headers, ensure that used == 0,
// otherwise we have a leak.
ym_errc
ym_mem_shutdown()
{
    free(ym_g_memory);
    return ym_errc_success;
}

ym_mem_region*
ym_mem_create_region(ym_mem_reg_id region,
                     uint16_t size)
{
    YM_ASSERT(region < ym_mem_reg_count,
              ym_errc_invalid_input,
              "Allocating from invalid region, %" PRIu8
              " size: %" PRIu16 "",
              region,
              size);

    ym_mem_region* meta = malloc(sizeof(ym_mem_region));

    meta->mem = malloc(size);
    meta->region = region;
    meta->size = size;
    meta->used = 0;

    return meta;
}

void
ym_mem_destroy_region(ym_mem_region* region)
{
    YM_ASSERT(region->region < ym_mem_reg_count,
              ym_errc_invalid_input,
              "Deallocating from invalid region, %" PRIu8
              " size: %" PRIu16 "",
              region->region,
              region->size);

    free(region->mem);
    free(region);
}
