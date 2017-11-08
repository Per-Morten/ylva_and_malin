#include <ym_error.h>

static ym_errc ym_g_errc;

void
ym_raise_error(ym_errc error)
{
    ym_g_errc = error;
}

ym_errc
ym_clear_error()
{
    ym_errc current = ym_g_errc;
    ym_g_errc = ym_errc_success;
    return current;
}

const char*
ym_errc_str(ym_errc errc)
{
    switch (errc)
    {
        case ym_errc_success:
            return "ym_errc_success";
        case ym_errc_invalid_input:
            return "ym_errc_invalid_input";
        case ym_errc_bad_alloc:
            return "ym_errc_bad_alloc";
        case ym_errc_mem_leak:
            return "ym_errc_mem_leak";
        case ym_errc_uninitialized:
            return "ym_errc_uninitialized";
        case ym_errc_system_error:
            return "ym_errc_system_error";
    }

    return "Unknown Error!";
}
