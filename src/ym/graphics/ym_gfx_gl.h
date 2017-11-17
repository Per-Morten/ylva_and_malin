#pragma once

#include <ym_core.h>
#include <ym_gfx.h>
#include <ym_gfx_gl_procs.h>


YM_NO_DISCARD
ym_errc
ym_gfx_gl_init();


YM_NO_DISCARD
ym_errc
ym_gfx_gl_create_shader(GLenum type,
                        const char* file_path,
                        GLuint* out_shader);

YM_NO_DISCARD
ym_errc
ym_gfx_gl_create_program(GLuint* shaders,
                         int shader_count,
                         GLuint* out_program);
