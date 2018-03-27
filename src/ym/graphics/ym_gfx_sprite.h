#pragma once
#include <ym_core.h>
#include <ym_memory.h>
#include <ym_gfx_gl.h>

YM_NO_DISCARD
ym_errc
ym_gfx_load_png(const char* filename,
                u8** out_image,
                int* out_width,
                int* out_height);

typedef GLuint ym_sheet_id;
typedef GLuint ym_sprite_id;

// Initialize all needed functionality for drawing sprites,
// Setup vbo, ibo, vao, shaders, texture_coordinates, etc.
YM_NO_DISCARD
ym_errc
ym_sprite_init(ym_mem_region* memory_region,
               ym_gfx_window* window);

YM_NO_DISCARD
ym_errc
ym_sprite_shutdown();

// Need to figure out how we can do allocations here,
// Do we need to support complex deallocations,
// or can we just force the loading of sprite sheets so that
//
YM_NO_DISCARD
ym_errc
ym_sprite_load_sheet(const char* filename,
                     uint col_count,
                     uint row_count,
                     ym_sheet_id* out_sheet_id);

YM_NO_DISCARD
ym_errc
ym_sprite_delete_sheet(ym_sheet_id sheet_id);

// Think if this should be done async, i.e. just add to some
// sort of draw container/queue.
// Cannot be a straight queue, as we need to keep layering information
// need to keep the container sorted on layering somehow.
// Should also be sorted as much as possible on the sheet_id and sprite_id as well,
// so that we don't need to do so many uniform calls.
/// \todo Add layering support
// This can actually be done async, in that case it is best if we also supply a present layer
// function, so I can decide layering and when they should be drawn, that way I can basically
// do my own buffering solutions.
ym_errc
ym_sprite_draw(ym_sheet_id sheet_id,
               ym_sprite_id sprite_id,
               uint layer,
               ym_vec2 pos);

/// \todo add layering support
ym_errc
ym_sprite_draw_extd(ym_sheet_id sheet_id,
                    ym_sprite_id sprite_id,
                    uint layer,
                    ym_vec2 pos,
                    ym_vec2 scale,
                    float angle);

// Will be used so we can start drawing async. As soon as this command is run,
// The render thread will start drawing this layer in the background if possible.
// Think about if this is really a good idea.
ym_errc
ym_sprite_commit_layer(uint layer);

void
ym_sprite_set_camera_pos(ym_vec3 camera_pos);
