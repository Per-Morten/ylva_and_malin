#include <ym_gfx_sprite.h>
#include <lodepng.h>



ym_errc
ym_gfx_load_png(const char* filename,
                u8** out_image,
                int* out_width,
                int* out_height)
{
    // TODO: Move over to using the lodepng function that takes in memory
    // so we can use our own memory locations.
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

struct
{
    uint row_count;
    uint col_count;
    ym_sheet_id id;
} g_ym_sprite_info[2];

ym_errc
ym_sprite_load_sheet(const char* filename,
                     uint col_count,
                     uint row_count,
                     ym_sheet_id* out_sheet_id)
{
    ym_errc errc = ym_gfx_gl_create_texture(filename,
                                            GL_TEXTURE0,
                                            out_sheet_id);

    if (errc != ym_errc_success)
        return errc;

    g_ym_sprite_info[*out_sheet_id].row_count = row_count;
    g_ym_sprite_info[*out_sheet_id].col_count = col_count;
    g_ym_sprite_info[*out_sheet_id].id = *out_sheet_id;

    return ym_errc_success;
}

