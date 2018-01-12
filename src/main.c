#include <stdbool.h>
#include <math.h>

#include <ym_core.h>
#include <ym_memory.h>
#include <ym_gfx.h>
#include <ym_gfx_gl.h>
#include <ym_gfx_sprite.h>
#include <ym_telemetry.h>

ym_errc
init_subsystems(ym_gfx_window* window)
{
    ym_errc errc = ym_errc_success;

    //errc |= ym_telemetry_init(ym_mem_get_region(ym_mem_reg_telemetry));
    errc |= ym_gfx_gl_init(ym_mem_reg_gl);
    errc |= ym_sprite_init(NULL, window);

    return errc;
}

ym_errc
shutdown_subsystems()
{
    ym_errc errc = ym_errc_success;

    errc |= ym_sprite_shutdown();
    //errc |= ym_gfx_gl_shutdown();
    //errc |= ym_telemetry_shutdown();

    return errc;
}

int
main(YM_UNUSED int argc,
     YM_UNUSED char** argv)
{
    ym_gfx_window* window = NULL;

    ym_errc errc = ym_mem_init();
    if (errc != ym_errc_success)
        goto cleanup;

    errc = ym_gfx_init(ym_mem_reg_gfx);
    if (errc != ym_errc_success)
        goto cleanup;

    window = ym_gfx_create_window(800, 600, "ylva_and_malin");


    errc = init_subsystems(window);
    if (errc != ym_errc_success)
        goto cleanup;

    ym_sheet_id malin_sheet;
    ym_sprite_load_sheet("resources/sprites/malin_regular.png", 3, 4,
                         &malin_sheet);

    ym_sheet_id ylva_sheet;
    ym_sprite_load_sheet("resources/sprites/ylva_regular.png", 3, 4,
                         &ylva_sheet);

    ym_sprite_id texture_id = 0;
    double dt = 0.0;
    float angle = YM_DEG_TO_RAD(0.0f);
    float anim_timer = 0.0f;

    int count = 0;
    ym_vec2 malin_pos =
    {
        .x = 400.0f,
        .y = 300.0f,
    };

    ym_vec2 ylva_pos =
    {
        .x = 400.0f,
        .y = 300.0f,
    };

    // TESTING MALLOCS
    // SERIOUSLY; CREATE MEMORY TESTS!!
    for (int k = 0; k < 2; k++)
    {
        void* memory[3];
        for (int i = 0; i < 3; i++)
            memory[i] = YM_MALLOC(ym_mem_reg_gfx, 40);

        for (int i = 0; i < 3; i++)
            YM_FREE(ym_mem_reg_gfx, 40, memory[i]);
        //for (int i = 2; i >= 0; i--)
        //    YM_FREE(ym_mem_reg_gfx, 40, memory[i]);


    }

    // EO TESTING MALLOCS


    while (ym_gfx_window_is_open(window))
    {
        double start = ym_clock_now();
        ym_gfx_window_poll_events(window);
        ym_gfx_window_clear(window);

        anim_timer += dt;
        if (anim_timer >= 0.25f)
        {
            anim_timer = 0.0f;
            texture_id++;
            texture_id = texture_id % 12;
        }


        // SUPER HAX FOR TESTING TRANSFORMATIONS!
        typedef
        struct
        {
            Display* display;
            Window win;
            bool is_open;
            u8 pad[7];

            bool w;
            bool a;
            bool s;
            bool d;
            bool e;
            bool q;
        } super_hax;

        super_hax* win_hax = window;

        if (win_hax->w)
        {
            malin_pos.y -= 100 * dt;
            //YM_DEBUG("w pressed, go up: %f", malin_pos.y);
        }
        if (win_hax->s)
        {
            malin_pos.y += 100 * dt;
            //YM_DEBUG("s pressed, go down: %f", malin_pos.y);
        }
        if (win_hax->a)
        {
            malin_pos.x -= 100 * dt;
            //YM_DEBUG("a pressed, go left: %f", malin_pos.x);
        }
        if (win_hax->d)
        {
            malin_pos.x += 100 * dt;
            //YM_DEBUG("d pressed, go right: %f", malin_pos.x);
        }
        if (win_hax->e)
        {
            angle -= 2 * dt;
            //YM_DEBUG("e pressed, rotate left");
        }
        if (win_hax->q)
        {
            angle += 2 * dt;
            //YM_DEBUG("q pressed, rotate right");
        }
        // EO Super hax for testing transformations

        ym_vec2 scale =
        {
            .x = 1.0f,
            .y = 1.0f,
        };
        ym_sprite_draw_extd(malin_sheet, texture_id, 0, malin_pos, scale, angle);
        ym_sprite_draw(ylva_sheet, texture_id, 0, ylva_pos);

        ym_gfx_window_display(window);

        double end = ym_clock_now();

        dt = end - start;

    }

    cleanup:
    errc = ym_errc_success;
    errc |= shutdown_subsystems();
    if ((errc != ym_errc_success))
        YM_WARN("Shutdown with error %s", ym_errc_str(errc));

    ym_gfx_destroy_window(window);

    errc = ym_mem_shutdown();

    YM_DEBUG("Shutting down");

    return 0;
}
