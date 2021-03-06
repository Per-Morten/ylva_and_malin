#include <stdbool.h>
#include <math.h>

#include <ym_core.h>
#include <ym_memory.h>
#include <ym_gfx.h>
#include <ym_gfx_gl.h>
#include <ym_gfx_sprite.h>
#include <ym_telemetry.h>
#include <ym_resource.h>

ym_errc
init_subsystems(ym_gfx_window* window)
{
    ym_errc errc = ym_errc_success;

    YM_INFO("Initializing Telemetry module");
    //errc |= ym_telemetry_init(ym_mem_get_region(ym_mem_reg_telemetry));

    YM_INFO("Initializing OpenGL module");
    errc |= ym_gfx_gl_init(ym_mem_reg_gl);

    YM_INFO("Initializing Sprite module");
    errc |= ym_sprite_init(NULL, window);

    return errc;
}

ym_errc
shutdown_subsystems()
{
    ym_errc errc = ym_errc_success;

    YM_INFO("Shutting down Sprite module");
    errc |= ym_sprite_shutdown();

    YM_INFO("Shutting down OpenGL module");
    //errc |= ym_gfx_gl_shutdown();

    YM_INFO("Shutting down Telemetry module");
    //errc |= ym_telemetry_shutdown();

    return errc;
}

int
main(YM_UNUSED int argc,
     YM_UNUSED char** argv)
{
    ym_gfx_window* window = NULL;

    YM_INFO("Initializing Memory module");
    ym_errc errc = ym_mem_init();
    if (errc != ym_errc_success)
        goto cleanup;

    YM_INFO("Initializing Gfx module");
    errc = ym_gfx_init(ym_mem_reg_gfx);
    if (errc != ym_errc_success)
        goto cleanup;

    YM_INFO("Creating window");
    errc = ym_gfx_create_window(800, 600, "ylva_and_malin", &window);
    if (errc != ym_errc_success)
        goto cleanup;

    errc = init_subsystems(window);
    if (errc != ym_errc_success)
        goto cleanup;


    ym_sheet_id malin_sheet;
    ym_sprite_load_sheet("resources/sprites/malin_regular.png", 3, 4,
                         &malin_sheet);

    ym_sheet_id ylva_sheet;
    ym_sprite_load_sheet("resources/sprites/ylva_regular.png", 3, 4,
                         &ylva_sheet);

    ym_tileset* tileset;
    int tilecount;
    ym_layer* layer;
    int layercount;
    ym_resource_load_map("resources/maps/testing.tmx",
                         &tileset,
                         &tilecount,
                         &layer,
                         &layercount);

    // Load sheets
    char filepath[256];
    for (int i = 0; i < tilecount; i++)
    {
        strcpy(filepath, "resources/sprites/");
        strcat(filepath, tileset[i].filename);
        ym_sprite_load_sheet(filepath,
                             tileset[i].rowcount,
                             tileset[i].colcount,
                             &tileset[i].sheet_id);
    }

    ym_resource_parse_map(tileset, tilecount, layer, layercount);

    ym_sprite_id texture_id = 0;
    double dt = 0.0;
    float angle = YM_DEG_TO_RAD(0.0f);
    float anim_timer = 0.0f;

    int count = 0;
    ym_vec2 malin_pos =
    {

        .x = 6.0f,
        .y = 7.0f,
    };

    ym_vec2 ylva_pos =
    {
        .x = 4.0f,
        .y = 4.0f,
    };

    ym_vec3 camera_pos =
    {
        .x = 0.0f,
        .y = 0.0f,
        .z = 2.0f,
    };

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
            //YM_DEBUG("texture_id: %d", texture_id);
        }

        // SUPER HAX FOR TESTING TRANSFORMATIONS!
        typedef
        struct
        {
            void* display;
            void* win;
            bool is_open;
            u8 pad[7];

            bool w;
            bool a;
            bool s;
            bool d;
            bool e;
            bool q;
            bool up;
            bool down;
            bool left;
            bool right;
        } super_hax;

        super_hax* win_hax = window;

        if (win_hax->w)
        {
            malin_pos.y -= 1 * dt;
            //YM_DEBUG("w pressed, go up: %f", malin_pos.y);
        }
        if (win_hax->s)
        {
            malin_pos.y += 1 * dt;
            //YM_DEBUG("s pressed, go down: %f", malin_pos.y);
        }
        if (win_hax->a)
        {
            malin_pos.x -= 1 * dt;
            //YM_DEBUG("a pressed, go left: %f", malin_pos.x);
        }
        if (win_hax->d)
        {
            malin_pos.x += 1 * dt;
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

        if (win_hax->up)
        {
            //YM_DEBUG("down pressed");
            camera_pos.y += 1 * dt;
        }
        if (win_hax->down)
        {
            //YM_DEBUG("down pressed");
            camera_pos.y -= 1 * dt;
        }
        if (win_hax->left)
        {
            //YM_DEBUG("left pressed");
            camera_pos.x -= 1 * dt;
        }
        if (win_hax->right)
        {
            //YM_DEBUG("right pressed");
            camera_pos.x += 1 * dt;
        }
        // EO Super hax for testing transformations

        ym_vec2 scale =
        {
            .x = 1.0f,
            .y = 1.0f,
        };

        for (int i = 0; i < layercount; i++)
        {
            for (int x = 0; x < 10; x++)
            {
                for (int y = 0; y < 10; y++)
                {
                    if (layer[i].sprite_ids[x][y] == 0)
                        continue;
                    ym_sprite_draw(layer[i].sheet_ids[x][y],
                                   layer[i].sprite_ids[x][y],
                                   0,
                                   layer[i].coordinates[x][y]);
                }
            }
//                ym_sprite_draw(floor_sheet, 0, 0, (ym_vec2){.x = i * 100.0f, .y = j * 100.0f});
        }

        ym_sprite_set_camera_pos((ym_vec3){.x = malin_pos.x, .y = -malin_pos.y, .z = camera_pos.z});
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

    YM_INFO("Destroying window");
    ym_gfx_destroy_window(window);

    YM_INFO("Shutting Down Memory module")
    errc = ym_mem_shutdown();

    YM_DEBUG("Shutting Down");

    return 0;
}
