#pragma once
#include <ym_attributes.h>

///////////////////////////////////////////////////////////
/// \enum ym_errc
/// \ingroup ym_core
///
/// \brief
///     Used to indicate erroneous behavior within a
///     a function.
///
/// \detailed
///     This type is either returned from a function,
///     or in some cases it is sent in as an
///     out parameter.
///
/// \var ym_errc::ym_errc_success
/// \brief
///     Indicates a successful operation.
///
/// \var ym_errc::ym_errc_invalid_input
/// \brief
///     Indicates that one of the parameters supplied
///     to the function was invalid.
///
/// \var ym_errc::ym_errc_bad_alloc
/// \brief
///     Indicates that requested memory could not be
///     allocated.
///
/// \var ym_errc::ym_errc_mem_leak
/// \brief
///     Indicates that memory has been leaked from a
///     region.
///////////////////////////////////////////////////////////
typedef
enum
{
    ym_errc_success         = (0 << 0),
    ym_errc_invalid_input   = (1 << 0),
    ym_errc_bad_alloc       = (1 << 1),
    ym_errc_mem_leak        = (1 << 2),
    ym_errc_uninitialized   = (1 << 3),
    ym_errc_system_error    = (1 << 4),
    ym_errc_gl_error        = (1 << 5),
    ym_errc_out_of_memory   = (1 << 6),
} ym_errc;


///////////////////////////////////////////////////////////
/// \ingroup ym_core
///
/// \brief
///     Sets the new global error within the ym project.
///     Only used for handling errors when testing.
///
/// \param errc
///     The error code to raise.
///////////////////////////////////////////////////////////
void
ym_raise_error(ym_errc errc);

///////////////////////////////////////////////////////////
/// \ingroup ym_core
///
/// \brief
///     Clears the global error and gets back its
///     previous values.
///     Only used for handling errors when testing.
///
///////////////////////////////////////////////////////////
YM_NO_DISCARD
ym_errc
ym_clear_error();

///////////////////////////////////////////////////////////
/// \ingroup ym_core
///
/// \brief
///     Translates an ym_errc to a human readable string.
///
/// \returns
///     Pointer to human readable string of errc.
///////////////////////////////////////////////////////////
YM_NO_DISCARD
const char*
ym_errc_str(ym_errc errc);
