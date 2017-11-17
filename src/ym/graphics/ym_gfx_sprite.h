#pragma once
#include <ym_core.h>

YM_NO_DISCARD
ym_errc
ym_gfx_load_png(const char* filename,
                u8** out_image,
                int* out_width,
                int* out_height);
