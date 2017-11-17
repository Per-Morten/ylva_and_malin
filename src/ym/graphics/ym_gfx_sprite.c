#include <ym_gfx_sprite.h>
#include <lodepng.h>

ym_errc
ym_gfx_load_png(const char* filename,
                u8** out_image,
                int* out_width,
                int* out_height)
{
    unsigned width = 0;
    unsigned height = 0;
    unsigned error = lodepng_decode32_file(out_image,
                                           &width,
                                           &height,
                                           filename);
    if (error)
    {
        YM_WARN("%s: lodepng error: %u: %s\n",
                ym_errc_str(ym_errc_system_error),
                error,
                lodepng_error_text(error));
        return ym_errc_system_error;
    }

    *out_width = width;
    *out_height = height;

    return ym_errc_success;
}
