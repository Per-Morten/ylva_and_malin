#include <ym_gfx.h>
#include <Windows.h>
#include <GL/gl.h>

static ym_mem_region* ym_gfx_mem_reg;

ym_errc
ym_gfx_init(ym_mem_region* memory_region)
{
    YM_ASSERT(memory_region,
              ym_errc_invalid_input,
              "memory_region cannot be NULL");

    ym_gfx_mem_reg = memory_region;
    return ym_errc_success;
}

ym_errc
ym_gfx_shutdown()
{
    // Free all resources
    return ym_errc_success;
}

ym_gfx_window*
ym_gfx_create_window(uint16_t width,
                     uint16_t height,
                     const char* window_name)
{
    YM_ASSERT(window_name,
              ym_errc_invalid_input,
              "Window name must not be NULL");

    return NULL;
}

void
ym_gfx_destroy_window(ym_gfx_window* w)
{
    //YM_ASSERT(w,
    //          ym_errc_invalid_input,
    //          "Window must not be NULL");
}

bool
ym_gfx_window_is_open(const ym_gfx_window* w)
{
    //YM_ASSERT(w,
    //          ym_errc_invalid_input,
    //          "Window must not be NULL");

    return false;
}

void
ym_gfx_window_display(ym_gfx_window* w)
{
    YM_ASSERT(w,
              ym_errc_invalid_input,
              "Window must not be NULL");

    // Currently just a mock function.
    // Only the glXSwapBuffers call will be left.
    // will be left when finished (I guess).
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1., 1., -1., 1., 1., 20.);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBegin(GL_QUADS);
    glColor3f(1., 0., 0.); glVertex3f(-.75, -.75, 0.);
    glColor3f(0., 1., 0.); glVertex3f( .75, -.75, 0.);
    glColor3f(0., 0., 1.); glVertex3f( .75,  .75, 0.);
    glColor3f(1., 1., 0.); glVertex3f(-.75,  .75, 0.);
    glEnd();

}

void
ym_gfx_window_poll_events(ym_gfx_window* w)
{
    YM_ASSERT(w,
              ym_errc_invalid_input,
              "Window must not be NULL");
}
