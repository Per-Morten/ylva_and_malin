#include <stdbool.h>

#include <ym_core.h>
#include <ym_memory.h>
#include <ym_gfx.h>
#include <ym_gfx_gl.h>
#include <ym_telemetry.h>
#include <ym_thread.h>

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

void
test_function(void* args)
{
    YM_DEBUG("%d", *(int*)args);
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

    if ((errc |= ym_gfx_init_gl()) != ym_errc_success)
    {
        ym_gfx_destroy_window(window);
        goto cleanup;
    }

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

    const char* vert_shader =
        "#version 410\n"
        "in vec3 vp;"
        "void main() {"
        "   gl_Position = vec4(vp, 1.0);"
        "}";

    const char* frag_shader =
        "#version 410\n"
        "out vec4 frag_colour;"
        "void main() {"
        "   frag_colour = vec4(0.5, 0.0, 0.5, 1.0);"
        "}";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vert_shader, NULL);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &frag_shader, NULL);
    glCompileShader(fs);

    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, fs);
    glAttachShader(shader_program, vs);
    glLinkProgram(shader_program);

    while (false && ym_gfx_window_is_open(window))
    {
        ym_gfx_window_poll_events(window);
        ym_gfx_window_clear(window);

        glUseProgram(shader_program);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);



        ym_gfx_window_display(window);
    }

    ym_gfx_destroy_window(window);

    if ((errc = shutdown_subsystems()) != ym_errc_success)
        YM_WARN("Shutdown not without error %s", ym_errc_str(errc));


    cleanup:
    errc = ym_mem_shutdown();

    int i = 1;
    ym_thread thread = ym_thread_create(test_function, &i);

    ym_thread_join(&thread);

    system("Pause");

    return 0;
}
