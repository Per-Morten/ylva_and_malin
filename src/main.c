#include <stdbool.h>

#include <ym_core.h>
#include <ym_memory.h>
#include <ym_gfx.h>
#include <ym_telemetry.h>

ym_errc
init_subsystems()
{
    ym_errc errc = ym_errc_success;

    errc |= ym_telemetry_init(ym_mem_get_region(ym_mem_reg_telemetry));
    errc |= ym_gfx_init(ym_mem_get_region(ym_mem_reg_gfx));

    return errc;
}

ym_errc
shutdown_subsystems()
{
    ym_errc errc = ym_errc_success;

    errc |= ym_gfx_shutdown();
    errc |= ym_telemetry_shutdown();

    return errc;
}

int
main(YM_UNUSED int argc,
     YM_UNUSED char** argv)
{
    ym_errc errc = ym_mem_init();

    if (errc |= ym_errc_success || init_subsystems() != ym_errc_success)
        goto cleanup;

    ym_gfx_window* window =
        ym_gfx_create_window(600, 600, "ylva_and_malin");

    while (ym_gfx_window_is_open(window))
    {
        ym_gfx_window_poll_events(window);
        ym_gfx_window_display(window);
    }

    ym_gfx_destroy_window(window);

    if ((errc = shutdown_subsystems()) != ym_errc_success)
        YM_WARN("Shutdown not without error %s", ym_errc_str(errc));


    cleanup:
    errc = ym_mem_shutdown();

    YM_DEBUG("Shutting down");

    return 0;
}
