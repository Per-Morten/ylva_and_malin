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
    GLfloat points[] =
    {
       0.0f, 0.5f, 0.0f,
       0.0f, 0.0f, 0.0f,
       0.5f, 0.0f, 0.0f,
       0.5f, 0.5f, 0.0f,
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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 6, indices, GL_STATIC_DRAW);

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

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

    float matrix[] =
    {
        1.0f, 0.0f, 0.0f, 0.0f, // first col
        0.0f, 1.0f, 0.0f, 0.0f, // second col
        0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.0f, 0.0f, 1.0f,
    };

    int count = 0;
    double dt = 0.0;
    while (ym_gfx_window_is_open(window))
    {
        double start = ym_clock_now();

        ym_gfx_window_poll_events(window);
        ym_gfx_window_clear(window);

        //glPolygonMode(GL_FRONT, GL_LINE);
        glUniform4f(color, 0.0f, 0.75f, 0.75f, 1.0f);
        glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, matrix);
        //glBindTexture(GL_TEXTURE_2D, (count++ % 2 == 0) ? tex : tex2);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
        //glActiveTexture(GL_TEXTURE1);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);


        double end = ym_clock_now();

        dt = end - start;

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
