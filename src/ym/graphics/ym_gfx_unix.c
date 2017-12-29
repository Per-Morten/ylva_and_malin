#include <ym_gfx.h>
#include <ym_gfx_gl.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>

static ym_mem_region* ym_gfx_mem_reg;

///////////////////////////////////////////////////////////
/// \struct ym_gfx_unix_window
/// \brief
///     For internal use only!
///
/// \internal
/// \detailed
///     Unix specific window representation that
///     ym_gfx_window is casted over to when
///     working on unix.
///
/// \see
///     https://www.khronos.org/opengl/wiki/Programming_OpenGL_in_Linux:_GLX_and_Xlib
///     https://stackoverflow.com/questions/19305982/xlib-closing-window-always-causes-fatal-io-error
///     https://stackoverflow.com/questions/10792361/how-do-i-gracefully-exit-an-x11-event-loop
///
/// \var ym_gfx_unix_window::display
/// \brief
///     The X11 display connection.
///
/// \var ym_gfx_unix_window::win
/// \brief
///     The X11 graphical window.
///
/// \var ym_gfx_unix_window::is_open
/// \brief
///     Bool indicating if window is still open.
///
/// \var ym_gfx_unix_window::pad
/// \brief
///     Simple explicit padding.
///////////////////////////////////////////////////////////
typedef
struct
{
    Display* display;
    Window win;
    bool is_open;
    u8 pad[7];

    /// \todo Remove this when getting sprite movement to work.
    /// Should have a better way of doing keyboard handling,
    /// this is just for hax.
    bool w;
    bool a;
    bool s;
    bool d;
    bool e;
    bool q;
    // EO keyboard Hax

    // After removed hax,
    // this must be evened out with the padding of the struct
    u16 width;
    u16 height;
} ym_gfx_unix_window;

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

    // Allocate from beginning of memory region
    ym_gfx_unix_window* window = ym_gfx_mem_reg->mem;
    ym_gfx_mem_reg->used += sizeof(ym_gfx_unix_window);

    // Using Xkb just so we can use XkbKeycodeToKeysym which isn't deprecated.
    int errc = 0;

    // In case we need to listen to xbk events, this is the id for that.
    // Store it in the window struct.
    int xbk_event_type = 0;
    window->display = XkbOpenDisplay(NULL, &xbk_event_type,
                                     NULL, NULL,
                                     NULL, &errc);

    if (errc != XkbOD_Success || window->display == NULL)
    {
        YM_WARN("Could not open display");
        return NULL;
    }

    Window root = DefaultRootWindow(window->display);

    // Rethink these attributes.
    GLint attributes[] =
    {
        GLX_RGBA,
        GLX_DEPTH_SIZE,
        24,
        GLX_DOUBLEBUFFER,
        None
    };

    XVisualInfo* visual_info = glXChooseVisual(window->display, 0, attributes);

    // Ensure proper error handling her.
    // Might leak as we are not doing XCloseDisplay on win->display
    if (visual_info == NULL)
    {
        XCloseDisplay(window->display);
        return NULL;
    }

    Colormap cmap = XCreateColormap(window->display, root,
                                    visual_info->visual,
                                    AllocNone);

    window->win = XCreateWindow(window->display, root,
                                0, 0,
                                width, height, 0,
                                visual_info->depth, InputOutput,
                                visual_info->visual, CWColormap | CWEventMask,
                                &(XSetWindowAttributes)
                                {
                                   .colormap = cmap,

                                   // Not listening to key or button events as these are polled manually
                                   .event_mask = ExposureMask
                                               | FocusChangeMask
                                               ,
                                });

    XMapWindow(window->display, window->win);
    XStoreName(window->display, window->win, window_name);

    // Sets us up to listen to the close window event, i.e. if the user presses
    // the x in the window.
    Atom delete_event = XInternAtom(window->display, "WM_DELETE_WINDOW", false);
    XSetWMProtocols(window->display, window->win, &delete_event, 1);

    GLXContext context = glXCreateContext(window->display,
                                          visual_info,
                                          NULL,
                                          GL_TRUE);

    // Changes the current context
    glXMakeCurrent(window->display, window->win, context);

    window->is_open = true;
    window->width = width;
    window->height = height;

    return window;
}

void
ym_gfx_destroy_window(ym_gfx_window* w)
{
    YM_ASSERT(w,
              ym_errc_invalid_input,
              "Window must not be NULL");

    ym_gfx_unix_window* window = w;
    GLXContext context = glXGetCurrentContext();

    glXMakeCurrent(window->display, None, NULL);
    glXDestroyContext(window->display, context);

    XDestroyWindow(window->display, window->win);
    XCloseDisplay(window->display);

    ym_gfx_mem_reg->used -= sizeof(ym_gfx_unix_window);
}

bool
ym_gfx_window_is_open(const ym_gfx_window* w)
{
    YM_ASSERT(w,
              ym_errc_invalid_input,
              "Window must not be NULL");

    return ((const ym_gfx_unix_window*)w)->is_open;
}

void
ym_gfx_window_get_size(ym_gfx_window* w,
                       uint* out_width,
                       uint* out_height)
{
    YM_ASSERT(w,
              ym_errc_invalid_input,
              "Window must not be NULL");

    ym_gfx_unix_window* win = w;
    *out_width = win->width;
    *out_height = win->height;
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

    // Currently just a mock function.
    // Only the glXSwapBuffers call will be left.
    // will be left when finished (I guess).
    ym_gfx_unix_window* window = w;

    glXSwapBuffers(window->display, window->win);
}

void
ym_gfx_window_poll_events(ym_gfx_window* w)
{
    YM_ASSERT(w,
              ym_errc_invalid_input,
              "Window must not be NULL");

    ym_gfx_unix_window* window = w;

    // XNextEvent blocks if no events has been received
    XFlush(window->display);
    int queued_events = XEventsQueued(window->display, QueuedAlready);

    // Process events
    for (int i = 0; i < queued_events; ++i)
    {
        XEvent event;
        XNextEvent(window->display, &event);

        switch (event.type)
        {
            case Expose:
            {
                // Do we really need to do this several times?
                // Or can we do it when we query for the mouse?
                XWindowAttributes attributes;
                XGetWindowAttributes(window->display, window->win, &attributes);
                window->width = attributes.width;
                window->height = attributes.height;
                glViewport(0, 0, attributes.width, attributes.height);
            }
            break;

            case ClientMessage:
            {
                if (event.xclient.data.l[0] == (long)XInternAtom(window->display,
                                                                 "WM_DELETE_WINDOW",
                                                                 true))
                {
                    window->is_open = false;
                }
            }
            break;

            case FocusIn:
            {
                YM_DEBUG("In Focus: Resume");
            }
            break;

            case FocusOut:
            {
                YM_DEBUG("Out of Focus: Pause");
            }
            break;
        }
    }

    // Poll keyboard
    char keys[32];
    XQueryKeymap(window->display, keys);

    // Hax keyboard "support"
    window->w = false;
    window->a = false;
    window->s = false;
    window->d = false;
    window->e = false;
    window->q = false;
    // eo Hax keyboard "support"


    for (int keycode = 0; keycode < 256; ++keycode)
    {
        if ((keys[keycode / 8] & (1 << (keycode % 8))) != 0)
        {
            // Hax keyboard "support"
            if (keycode == 38)
                window->a = true;
            if (keycode == 39)
                window->s = true;
            if (keycode == 25)
                window->w = true;
            if (keycode == 40)
                window->d = true;
            if (keycode == 26)
                window->e = true;
            if (keycode == 24)
                window->q = true;
            // eo hax keyboard "support"
            //YM_DEBUG("Key pressed: %d", keycode);
            //KeySym keysym = XkbKeycodeToKeysym(window->display, keycode, 0, 0);
            //YM_DEBUG("Keysym: %d", keysym);
            //YM_DEBUG("Name: %s", XKeysymToString(keysym));
        }
    }

    // Poll cursor
    Window root;
    Window child;
    int rootx;
    int rooty;
    int winx;
    int winy;
    unsigned int mask;

    if (XQueryPointer(window->display,
                      DefaultRootWindow(window->display),
                      &root, &child,
                      &rootx, &rooty,
                      &winx, &winy,
                      &mask))
    {
        XWindowAttributes root_attributes;
        XGetWindowAttributes(window->display, root, &root_attributes);

        //YM_DEBUG("Mouse pos: x: %d y: %d, button L: %d Button M: %d Button R: %d",
        //         root_attributes.x + rootx,
        //         root_attributes.y + rooty,
        //         (mask & Button1Mask),
        //         (mask & Button2Mask),
        //         (mask & Button3Mask));
    }
}
