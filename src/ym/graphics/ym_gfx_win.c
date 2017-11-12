#include <ym_gfx.h>
#include <Windows.h>
#include <Winuser.h>
#include <GL/gl.h>

// Not from windows gl, but from header files in directory.
#include <glext.h>
#include <wglext.h>

static ym_mem_region* ym_gfx_mem_reg;

typedef
struct
{
    HWND win;
    HDC gl_dc;
    HGLRC gl_rc;
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

            // Ugly hack to get this message properly!
            // Can probably search for this handle in the memory?
            ((ym_gfx_win_window*)ym_gfx_mem_reg->mem)->is_open = false;
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

    WNDCLASSEX win_class = {0};
    win_class.lpfnWndProc = window_procedure;
    win_class.hInstance = GetModuleHandle(NULL);
    win_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    win_class.cbSize = sizeof(WNDCLASSEX);
    win_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    win_class.lpszClassName = "ym_win_class";

    if (!RegisterClassEx(&win_class))
        YM_ERROR("Could not register class, error: %u", GetLastError());

    // Setup face window for device context.
    HWND fake_win = CreateWindow("ym_win_class", "Fake",
                                 WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                                 0, 0,
                                 1, 1,
                                 NULL, NULL,
                                 win_class.hInstance, NULL);

    if (!fake_win)
        YM_ERROR("Could not create fake window, error: %u", GetLastError());

    HDC fake_dc = GetDC(fake_win);

    PIXELFORMATDESCRIPTOR fake_pfd = {0};
    fake_pfd.nSize = sizeof(fake_pfd);
    fake_pfd.nVersion = 1;
    fake_pfd.dwFlags = PFD_DRAW_TO_WINDOW
                     | PFD_SUPPORT_OPENGL
                     | PFD_DOUBLEBUFFER;

    fake_pfd.iPixelType = PFD_TYPE_RGBA;
    fake_pfd.cColorBits = 32;
    fake_pfd.cAlphaBits = 8;
    fake_pfd.cDepthBits = 24;

    int fake_pfd_id = ChoosePixelFormat(fake_dc,
                                        &fake_pfd);
    if (!fake_pfd_id)
        YM_ERROR("Could not create fake pfd id, error: %u", GetLastError());

    if (!SetPixelFormat(fake_dc, fake_pfd_id, &fake_pfd))
        YM_ERROR("Could not set fake pixel format, error: %u", GetLastError());

    HGLRC fake_rc = wglCreateContext(fake_dc);
    if (!fake_rc)
        YM_ERROR("Could not set fake pixel format, error: %u", GetLastError());

    if (!wglMakeCurrent(fake_dc, fake_rc))
        YM_ERROR("Could not set current context, error: %u", GetLastError());

    // Get access to pixel format and create context.
    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB
        = wglGetProcAddress("wglChoosePixelFormatARB");

    if (!wglChoosePixelFormatARB)
    {
        YM_ERROR("Could not get proc address of \%s\", error: %u",
                 "wglChoosePixelFormatARB",
                 GetLastError());
    }

    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB
        = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

    if (!wglCreateContextAttribsARB)
    {
        YM_ERROR("Could not get proc address of \%s\", error: %u",
                 "wglCreateContextAttribsARB",
                 GetLastError());
    }

    ym_gfx_win_window* window = ym_gfx_mem_reg->mem;
    ym_gfx_mem_reg->used += sizeof(ym_gfx_win_window);

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

    if (!window->win)
        YM_ERROR("Could not create window, error: %u", GetLastError());

    window->gl_dc = GetDC(window->win);

    const int pixel_attribs[] =
    {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
        WGL_COLOR_BITS_ARB, 32,
        WGL_ALPHA_BITS_ARB, 8,
        WGL_DEPTH_BITS_ARB, 24,
        WGL_STENCIL_BITS_ARB, 8,
        WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
        WGL_SAMPLES_ARB, 4,
        0,
    };

    int pixel_fmt_id;
    UINT fmt_count;
    bool res = wglChoosePixelFormatARB(window->gl_dc, pixel_attribs, NULL,
                                       1, &pixel_fmt_id, &fmt_count);
    if (!res || !fmt_count)
        YM_ERROR("wglChoosePixelFormatARB failed.");

    PIXELFORMATDESCRIPTOR pfd;
    DescribePixelFormat(window->gl_dc, pixel_fmt_id, sizeof(pfd), &pfd);
    SetPixelFormat(window->gl_dc, pixel_fmt_id, &pfd);

    int context_attribs[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 5,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0,
    };

    window->gl_rc = wglCreateContextAttribsARB(window->gl_dc, 0, context_attribs);
    if (!window->gl_rc)
        YM_ERROR("wglCreateContextAttribsARB failed.");

    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(fake_rc);
    ReleaseDC(fake_win, fake_dc);
    DestroyWindow(fake_win);
    if (!wglMakeCurrent(window->gl_dc, window->gl_rc))
        YM_ERROR("wglMakeCurrent failed.");

    // Init rest of openGL here, or call function to do it, as it will probably ble insanely much.
    SetWindowText(window->win, (LPCSTR)glGetString(GL_VERSION));
    ShowWindow(window->win, SW_SHOWDEFAULT);

    window->is_open = true;
    return window;
}

void
ym_gfx_destroy_window(ym_gfx_window* w)
{
    YM_ASSERT(w,
              ym_errc_invalid_input,
              "Window must not be NULL");

    wglMakeCurrent(NULL, NULL);
    ym_gfx_win_window* window = w;
    wglDeleteContext(window->gl_rc);
    ReleaseDC(window->win, window->gl_dc);
    DestroyWindow(window->win);

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
ym_gfx_window_clear(YM_UNUSED ym_gfx_window* w)
{
    YM_ASSERT(w,
              ym_errc_invalid_input,
              "Window must not be NULL");

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void
ym_gfx_window_display(ym_gfx_window* w)
{
    YM_ASSERT(w,
              ym_errc_invalid_input,
              "Window must not be NULL");

    SwapBuffers(((ym_gfx_win_window*)w)->gl_dc);
}

void
ym_gfx_window_poll_events(ym_gfx_window* w)
{
    YM_ASSERT(w,
              ym_errc_invalid_input,
              "Window must not be NULL");

    ym_gfx_win_window* win = w;
    MSG msg;
    while (PeekMessage(&msg, win->win, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
