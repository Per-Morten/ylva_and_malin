#include <ym_gfx.h>
#include <Windows.h>
#include <Winuser.h>
#include <GL/gl.h>

static ym_mem_region* ym_gfx_mem_reg;

typedef
struct
{
    HWND win;
    bool is_open;
    u8 pad[7];
} ym_gfx_win_window;

// Unsure about what to do with this function really,
// because I actually want to do this processing in the poll_events
// function instead.
// At least do some of it there.
static
LRESULT
CALLBACK
window_procedure(HWND handle,
                 UINT msg,
                 WPARAM w_param,
                 LPARAM l_param)
{
    LRESULT res = 0;

    switch (msg)
    {
        case WM_ACTIVATE:
            YM_DEBUG("WM_Activate");
            break;

        case WM_CREATE:
            YM_DEBUG("WM_Create");
            break;

        case WM_DESTROY:
            YM_DEBUG("WM_Destroy");
            break;
        default:
            res = DefWindowProc(handle, msg, w_param, l_param);
    }
    return res;
}

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
ym_gfx_create_window(u16 width,
                     u16 height,
                     const char* window_name)
{
    YM_ASSERT(window_name,
              ym_errc_invalid_input,
              "Window name must not be NULL");

    ym_gfx_win_window* window = ym_gfx_mem_reg->mem;
    ym_gfx_mem_reg->used += sizeof(ym_gfx_win_window);

    WNDCLASSEX win_class = {0};
    win_class.lpfnWndProc = window_procedure;
    win_class.hInstance = GetModuleHandle(NULL);
    win_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    win_class.cbSize = sizeof(WNDCLASSEX);
    win_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    win_class.lpszClassName = "ym_win_class";

    if (!RegisterClassEx(&win_class))
        YM_ERROR("Could not register class, error: %u", GetLastError());

    const DWORD win_style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    RECT win_rect = {0, 0, width, height};
    if (!AdjustWindowRectEx(&win_rect, win_style, false, 0))
        YM_ERROR("Could not adjust window rect, error: %u", GetLastError());

    window->win = CreateWindowEx(0, win_class.lpszClassName,
                                 window_name, win_style,
                                 CW_USEDEFAULT, CW_USEDEFAULT,
                                 win_rect.right - win_rect.left,
                                 win_rect.bottom - win_rect.top,
                                 NULL, NULL, win_class.hInstance, NULL);

    YM_ASSERT(window->win,
              ym_errc_system_error,
              "Could not create window, error: %u",
              GetLastError());

    ShowWindow(window->win, SW_SHOWDEFAULT);
    HDC context = GetDC(window->win);
    PatBlt(context, 0, 0, width, height, BLACKNESS);
    ReleaseDC(window->win, context);

    return window;
}

void
ym_gfx_destroy_window(ym_gfx_window* w)
{
    YM_ASSERT(w,
              ym_errc_invalid_input,
              "Window must not be NULL");

    DestroyWindow(((ym_gfx_win_window*)w)->win);
    ym_gfx_mem_reg->used -= sizeof(ym_gfx_win_window);
}

bool
ym_gfx_window_is_open(const ym_gfx_window* w)
{
    YM_ASSERT(w,
              ym_errc_invalid_input,
              "Window must not be NULL");

    return ((ym_gfx_win_window*)w)->is_open;
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
