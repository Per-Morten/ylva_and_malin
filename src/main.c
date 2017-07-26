#include <stdbool.h>

#include <ym_core.h>
#include <ym_memory.h>
#include <ym_gfx.h>

bool
init_subsystems(ym_mem_region* memory_regions)
{
    ym_errc errc = ym_errc_success;

    errc |= ym_gfx_init(memory_regions);

    return errc == ym_errc_success;
}

int
main(YM_UNUSED int argc,
     YM_UNUSED char** argv)
{
    ym_errc errc = ym_mem_init();
    ym_mem_region* graphics_meta = ym_mem_create_region(ym_mem_reg_gfx, 2048);

    if (errc != ym_errc_success || !init_subsystems(graphics_meta))
        goto cleanup;

    ym_gfx_window* window =
        ym_gfx_create_window(600, 600, "ylva_and_malin");

    while (ym_gfx_window_is_open(window))
    {
        ym_gfx_window_poll_events(window);
        ym_gfx_window_display(window);
    }

    ym_gfx_destroy_window(window);
    ym_mem_destroy_region(graphics_meta);

    cleanup:
    errc = ym_mem_shutdown();

    YM_DEBUG("Shutting down");

    return 0;
}
