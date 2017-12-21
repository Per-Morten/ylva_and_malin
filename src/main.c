#include <stdbool.h>

#include <ym_core.h>
#include <ym_memory.h>
#include <ym_gfx.h>
#include <ym_gfx_gl.h>
#include <ym_gfx_sprite.h>
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

    if (errc != ym_errc_success || init_subsystems() != ym_errc_success)
        goto cleanup;

    ym_gfx_window* window =
        ym_gfx_create_window(800, 600, "ylva_and_malin");

    if ((errc |= ym_gfx_gl_init()) != ym_errc_success)
    {
        ym_gfx_destroy_window(window);
        goto cleanup;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_CULL_FACE);
    //glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    // Turn this into a square
    // Also use Index Buffer Object
    // So you just can say which indexes to use.
    // Square is now placed around origin
    // Create square and load textures
    GLfloat points[] =
    {
       -0.25f,  0.25f,
       -0.25f, -0.25f,
        0.25f, -0.25f,
        0.25f,  0.25f,
    };

    GLuint indices[] =
    {
        0, 1, 2,
        0, 2, 3,
    };

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    GLuint ibo = 0;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    GLuint shaders[2];
    ym_gfx_gl_create_shader("resources/shaders/simple_shader.vert",
                            GL_VERTEX_SHADER,
                            &shaders[0]);

    ym_gfx_gl_create_shader("resources/shaders/simple_shader.frag",
                            GL_FRAGMENT_SHADER,
                            &shaders[1]);

    GLuint program;
    ym_gfx_gl_create_program(shaders, 2, &program);

    GLint color = glGetUniformLocation(program, "u_color");
    GLint matrix_loc = glGetUniformLocation(program, "u_matrix");
    GLint texture_id_loc = glGetUniformLocation(program, "u_texture_id");

    glUseProgram(program);
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    GLuint tex = 0;
    ym_gfx_gl_create_texture("resources/sprites/malin_regular.png",
                             GL_TEXTURE0,
                             &tex);

    GLuint tex2 = 0;
    ym_gfx_gl_create_texture("resources/sprites/ylva_regular.png",
                             GL_TEXTURE0,
                             &tex2);

    GLfloat tex_coords[] =
    {
       0.0f, 1.0f,
       0.0f, 0.0f,
       1.0f, 0.0f,
       1.0f, 1.0f,
    };

    GLuint vt_vbo = 0;
    glGenBuffers(1, &vt_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vt_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coords), tex_coords, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);
    // EO create square and load textures

    ym_vec4 pos =
    {
        .x = 400.0f,
        .y = 300.0f,
        .z = 0.0f,
        .w = 0.0f,
    };

    GLint texture_id = 0;
    double dt = 0.0;
    float angle = YM_DEG_TO_RAD(0.0f);
    float anim_timer = 0.0f;
    glUniform4f(color, 0.0f, 0.75f, 0.75f, 1.0f);

    // Simple view, allowing me to work in [0-2] space, rather than [-1 - 1]
    ym_mat4 view =
    {
        .val =
        {
             1.0f,  0.0f,  0.0f,  0.0f,
             0.0f,  1.0f,  0.0f,  0.0f,
             0.0f,  0.0f,  1.0f,  0.0f,
            -1.0f,  1.0f,  0.0f,  1.0f,
        },
    };

    int count = 0;
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
            YM_DEBUG("texture_id: %d", (int)texture_id);
            glUniform1i(texture_id_loc, texture_id);
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
            pos.y -= 100 * dt;
            YM_DEBUG("w pressed, go up: %f", pos.y);
        }
        if (win_hax->s)
        {
            pos.y += 100 * dt;
            YM_DEBUG("s pressed, go down: %f", pos.y);
        }
        if (win_hax->a)
        {
            pos.x -= 100 * dt;
            YM_DEBUG("a pressed, go left: %f", pos.x);
        }
        if (win_hax->d)
        {
            pos.x += 100 * dt;
            YM_DEBUG("d pressed, go right: %f", pos.x);
        }
        if (win_hax->e)
        {
            angle -= 2 * dt;
            YM_DEBUG("e pressed, rotate left");
        }
        if (win_hax->q)
        {
            angle += 2 * dt;
            YM_DEBUG("q pressed, rotate right");
        }
        // EO Super hax for testing transformations

        const float c = cosf(angle);
        const float s = sinf(angle);
        ym_vec4 pos2 =
        {
            .x = pos.x / 800.0f * 2.0f,
            .y = pos.y / 600.0f * -2.0f,
            .z = 0.0f,
            .w = 1.0f,
        };

        ym_mat4 translate = ym_translate_vec4(pos2);
        ym_mat4 rotate =
        {
            .val =
            {
                c,    s,    0.0f, 0.0f,
                -s,   c,    0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f,
            },
        };

        ym_mat4 model = ym_mul_mat4_mat4(rotate, translate);
        ym_mat4 res = ym_mul_mat4_mat4(model, view);

        //glPolygonMode(GL_FRONT, GL_LINE);
        glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, res.val);
        //glBindTexture(GL_TEXTURE_2D, (count++ % 2 == 0) ? tex : tex2);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
        //glActiveTexture(GL_TEXTURE1);
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

        ym_gfx_window_display(window);

        double end = ym_clock_now();

        dt = end - start;

    }

    ym_gfx_destroy_window(window);

    if ((errc = shutdown_subsystems()) != ym_errc_success)
        YM_WARN("Shutdown not without error %s", ym_errc_str(errc));


    cleanup:
    errc = ym_mem_shutdown();

    YM_DEBUG("Shutting down");

    return 0;
}
