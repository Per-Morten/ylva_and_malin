#pragma once
#include <stdbool.h>

#include <ym_core.h>
#include <ym_memory.h>

///////////////////////////////////////////////////////////
/// \ingroup ym_gfx
///
/// \brief
///     Simple typedef for void, as no direct handling
///     of the window is legal.
///
/// \detailed
///     To keep the window system cross platform
///     it has been decided that the window system
///     should be completely opaque.
///     And access is only given through functions
///     that accepts a ym_gfx_window* as a parameter.
///
///////////////////////////////////////////////////////////
typedef void ym_gfx_window;

///////////////////////////////////////////////////////////
/// \ingroup ym_gfx
///
/// \brief
///     Initializes the ym_gfx module with a given
///     memory region.
///
/// \detailed
///     Initializes the ym_gfx module, allowing it to
///     do any needed setup before it can function
///     properly.
///
/// \param memory_region
///     Pointer to the memory region the ym_gfx module
///     should use for allocating memory.
///     Cannot be NULL.
///     The region is stored, and cannot be deleted
///     before ym_gfx is shut down.
///
/// \returns
///     ym_errc_success on success.
///     ym_errc_* on failure.
///
///////////////////////////////////////////////////////////
YM_NO_DISCARD
ym_errc
ym_gfx_init(ym_mem_reg_id memory_region);

///////////////////////////////////////////////////////////
/// \ingroup ym_gfx
///
/// \brief
///     Finished the last of de-initialization needed
///     by the ym_gfx module.
///
/// \detailed
///     De-initializes the ym_gfx module.
///     Doing any required cleanup before shutting down.
///
/// \returns
///     ym_errc_success on success.
///     ym_errc_* on failure.
///////////////////////////////////////////////////////////
YM_NO_DISCARD
ym_errc
ym_gfx_shutdown();

///////////////////////////////////////////////////////////
/// \ingroup ym_gfx
///
/// \brief
///     Creates a ym_gfx_window with the specified
///     width, height, and name.
///
/// \detailed
///     Allocates and prepares a new window with the
///     supplied parameters.
///
/// \param width
///     The width of the window to create in pixels
///
/// \param height
///     The height of the window to create in pixels.
///
/// \param window_name
///     The name of the window to be created.
///     Cannot be NULL.
///
/// \returns
///     Pointer to newly allocated window on success,
///     NULL on failures.
///
/// \todo
///     Find out how to do error checking here.
///     Don't want to change the typedef to void* rather
///     than void.
///     As the cast is easier to have mistakes then.
///     Potentially do a out_errc?
///////////////////////////////////////////////////////////
YM_NO_DISCARD
ym_errc
ym_gfx_create_window(u16 width,
                     u16 height,
                     const char* window_name,
                     ym_gfx_window** out_window);

///////////////////////////////////////////////////////////
/// \ingroup ym_gfx
///
/// \brief
///     Destroys/deallocates the window pointed
///     to by window.
///
/// \param window
///     Window to destroy/deallocate.
///     Cannot be NULL.
///////////////////////////////////////////////////////////
void
ym_gfx_destroy_window(ym_gfx_window* window);

///////////////////////////////////////////////////////////
/// \ingroup ym_gfx
///
/// \brief
///     Checks if the current supplied window is open,
///     or if it has been closed.
///
/// \param window
///     The window to check if open.
///     Cannot be NULL.
///
/// \returns
///     True if the window is open.
///     False if it has been closed.
///////////////////////////////////////////////////////////
YM_NO_DISCARD
bool
ym_gfx_window_is_open(const ym_gfx_window* window);

///////////////////////////////////////////////////////////
/// \ingroup ym_gfx
///
/// \brief
///     Gets the width and height dimension of the given
///     window.
///
/// \param window
///     The window to check. Cannot be NULL.
///
/// \param out_width
///     Out parameter used to store width.
///
/// \param out_height
///     Out parameter used to store height.
///////////////////////////////////////////////////////////
void
ym_gfx_window_get_size(ym_gfx_window* window,
                       uint* out_width,
                       uint* out_height);


///////////////////////////////////////////////////////////
/// \ingroup ym_gfx
///
/// \brief
///     Clears the given window.
///
/// \param window
///     The window to be cleared.
///////////////////////////////////////////////////////////
void
ym_gfx_window_clear(ym_gfx_window* window);

///////////////////////////////////////////////////////////
/// \ingroup ym_gfx
///
/// \brief
///     Displays everything that has been rendered
///     to the window.
///
/// \detailed
///     Essentially it swaps the gl buffer.
///
/// \param window
///     The window to display to.
///     Cannot be NULL.
///////////////////////////////////////////////////////////
void
ym_gfx_window_display(ym_gfx_window* window);

///////////////////////////////////////////////////////////
/// \ingroup ym_gfx
///
/// \brief
///     Polls events tied to the window in question.
///
/// \detailed
///     Polls all events that the OS pushed.
///     Additionally the keyboard and mouse is updated.
///
/// \param window
///     Window to poll events from.
///     Cannot be NULL.
///////////////////////////////////////////////////////////
void
ym_gfx_window_poll_events(ym_gfx_window* window);
