#include <stdbool.h>

#include <ym_core.h>
#include <ym_memory.h>
#include <ym_gfx.h>
#include <ym_gfx_gl.h>
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
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    GLfloat points[] =
    {
        0.0f,  0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
       -0.5f, -0.5f, 0.0f,
    };

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    GLuint shaders[2];
    ym_gfx_gl_create_shader(GL_VERTEX_SHADER,
                            "resources/shaders/simple_shader.vert",
                            &shaders[0]);

    ym_gfx_gl_create_shader(GL_FRAGMENT_SHADER,
                            "resources/shaders/simple_shader.frag",
                            &shaders[1]);

    GLuint program;
    ym_gfx_gl_create_program(shaders, 2, &program);

    GLint color = glGetUniformLocation(program, "in_color");

    glUseProgram(program);
    glBindVertexArray(vao);

    while (ym_gfx_window_is_open(window))
    {
        ym_gfx_window_poll_events(window);
        ym_gfx_window_clear(window);

        glUniform4f(color, 1.0f, 0.0f, 0.0f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, 3);

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
