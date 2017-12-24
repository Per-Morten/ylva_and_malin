#pragma once

#include <ym_core.h>
#include <ym_gfx.h>
#include <ym_gfx_gl_procs.h>


YM_NO_DISCARD
ym_errc
ym_gfx_gl_init();


YM_NO_DISCARD
ym_errc
ym_gfx_gl_create_shader(const char* file_path,
                        GLenum type,
                        GLuint* out_shader);

YM_NO_DISCARD
ym_errc
ym_gfx_gl_create_program(GLuint* shaders,
                         int shader_count,
                         GLuint* out_program);

YM_NO_DISCARD
ym_errc
ym_gfx_gl_create_texture(const char* file_path,
                         GLenum texture_slot,
                         GLuint* out_texture);

YM_NO_DISCARD
ym_errc
ym_gfx_gl_get_uniform(GLuint program,
                      const char* uniform_name,
                      GLint* out_uniform);
