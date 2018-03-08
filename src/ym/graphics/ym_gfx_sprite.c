#include <ym_gfx_sprite.h>
#include <lodepng.h>

// Should this be here, or should it be moved to another file?
// Want to handle file etc outside of this area, and also send in memory.
// This function should just be a wrapper for lodepng, nothing extra.
ym_errc
ym_gfx_load_png(const char* filename,
                u8** out_image,
                int* out_width,
                int* out_height)
{
    /// \todo Move over to using the lodepng function that takes in memory
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

static ym_mem_region* ym_gfx_sprite_mem_reg;
static ym_gfx_window* g_window;

// Keep track of resources that needs to be deleted
static struct
{
    GLuint vbo;
    GLuint ibo;
    GLuint ibo_size;
    GLuint vao;
    GLuint texture_vbo;
    GLuint shaders[2];
    GLuint shader_program;
    struct
    {
        GLint color;
        GLint mvp; // Not really mvp yet, just model and view.
        GLint texture_id;
        GLint atlas_col_count;
        GLint atlas_row_count;
    } uniforms;

} g_ym_render_cfg;

// This system needs to be way more robust! Should also probably be dynamically allocated instead
// Also can't directly map ym_sheet_id to index in array, because it is not guaranteed to be ever incrementing, and also don't start at 0.
// Current hack is to have 3 slots. But this NEEDS to be changed!
static struct
{
    uint row_count;
    uint col_count;
    ym_sheet_id id;
} g_ym_sprite_info[3];


// Simple view, allowing me to work in [0-1] space, rather than [-1 - 1]
static const ym_mat4 ym_view_mat =
{
    .val =
    {
        1.0f,  0.0f,  0.0f,  0.0f,
        0.0f,  1.0f,  0.0f,  0.0f,
        0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.0f,  1.0f,
    },
};


ym_errc
ym_sprite_init(ym_mem_region* region, ym_gfx_window* window)
{
    ym_gfx_sprite_mem_reg = region;
    g_window = window;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

    // Should happen in window instead, so it can be controlled by a parameter
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    // Load Shaders
    ym_errc errc = ym_errc_success;

    errc |= ym_gfx_gl_create_shader("resources/shaders/simple_shader.vert",
                                    GL_VERTEX_SHADER,
                                    &g_ym_render_cfg.shaders[0]);

    errc |= ym_gfx_gl_create_shader("resources/shaders/simple_shader.frag",
                                    GL_FRAGMENT_SHADER,
                                    &g_ym_render_cfg.shaders[1]);

    errc |= ym_gfx_gl_create_program(g_ym_render_cfg.shaders, 2,
                                     &g_ym_render_cfg.shader_program);

    if (errc != ym_errc_success)
        return errc;

    glUseProgram(g_ym_render_cfg.shader_program);

    // Setup uniforms, ignoring return value on purpose
    // Don't want to crash or stop if we cannot find a uniform, at least not yet.
    errc |= ym_gfx_gl_get_uniform(g_ym_render_cfg.shader_program,
                                  "u_color",
                                  &g_ym_render_cfg.uniforms.color);

    errc |= ym_gfx_gl_get_uniform(g_ym_render_cfg.shader_program,
                                  "u_matrix",
                                  &g_ym_render_cfg.uniforms.mvp);

    errc |= ym_gfx_gl_get_uniform(g_ym_render_cfg.shader_program,
                                  "u_texture_id",
                                  &g_ym_render_cfg.uniforms.texture_id);

    errc |= ym_gfx_gl_get_uniform(g_ym_render_cfg.shader_program,
                                  "u_atlas_col_count",
                                  &g_ym_render_cfg.uniforms.atlas_col_count);

    errc |= ym_gfx_gl_get_uniform(g_ym_render_cfg.shader_program,
                                  "u_atlas_row_count",
                                  &g_ym_render_cfg.uniforms.atlas_row_count);

    // Create VAO
    glGenVertexArrays(1, &g_ym_render_cfg.vao);
    glBindVertexArray(g_ym_render_cfg.vao);

    // Create square to render sprites on
    GLfloat points[] =
    {
       -0.25f,  0.25f,
       -0.25f, -0.25f,
        0.25f, -0.25f,
        0.25f,  0.25f,
    };

    // Create VBO and assign it to vao
    glGenBuffers(1, &g_ym_render_cfg.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, g_ym_render_cfg.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    // Setup texture coordinates
    GLfloat tex_coords[] =
    {
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
    };

    // Create texture_vbo and assing it to vao
    glGenBuffers(1, &g_ym_render_cfg.texture_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, g_ym_render_cfg.texture_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coords), tex_coords, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    // Setup indices for ibo
    GLuint indices[] =
    {
        0, 1, 2,
        0, 2, 3,
    };
    g_ym_render_cfg.ibo_size = 6;

    glGenBuffers(1, &g_ym_render_cfg.ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ym_render_cfg.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    return ym_errc_success;
}

ym_errc
ym_sprite_shutdown()
{
    glDeleteBuffers(1, &g_ym_render_cfg.texture_vbo);
    glDeleteVertexArrays(1, &g_ym_render_cfg.vao);
    glDeleteBuffers(1, &g_ym_render_cfg.ibo);
    glDeleteBuffers(1, &g_ym_render_cfg.vbo);
    glUseProgram(0);
    glDeleteShader(g_ym_render_cfg.shaders[0]);
    glDeleteShader(g_ym_render_cfg.shaders[1]);
    glDeleteProgram(g_ym_render_cfg.shader_program);

    memset(&g_ym_render_cfg, 0, sizeof(g_ym_render_cfg));

    return ym_errc_success;
}

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

    // Find better way of doing this. Should allocate these resources
    // somewhere else.
    g_ym_sprite_info[*out_sheet_id].row_count = row_count;
    g_ym_sprite_info[*out_sheet_id].col_count = col_count;
    g_ym_sprite_info[*out_sheet_id].id = *out_sheet_id;

    return ym_errc_success;
}

ym_errc
ym_sprite_delete_sheet(ym_sheet_id sheet_id)
{
    glDeleteTextures(1, &g_ym_sprite_info[sheet_id].id);

    return ym_errc_success;
}

ym_errc
ym_sprite_draw(ym_sheet_id sheet_id,
               ym_sprite_id sprite_id,
               uint layer,
               ym_vec2 pos)
{
    uint w_width;
    uint w_height;
    ym_gfx_window_get_size(g_window, &w_width, &w_height);

    ym_vec4 real_pos =
    {
        .x = pos.x / w_width * 1.0f,
        .y = pos.y / w_height * -1.0f,
        .z = 0.0f,
        .w = 1.0f,
    };

    ym_mat4 translate = ym_translate_vec4(real_pos);
    ym_mat4 res = ym_mul_mat4_mat4(translate, ym_view_mat);
    glUniformMatrix4fv(g_ym_render_cfg.uniforms.mvp, 1, GL_FALSE, res.val);

    glBindTexture(GL_TEXTURE_2D, sheet_id);
    glUniform1ui(g_ym_render_cfg.uniforms.texture_id, sprite_id);

    glDrawElements(GL_TRIANGLES, g_ym_render_cfg.ibo_size, GL_UNSIGNED_INT, NULL);

    return ym_errc_success;
}

ym_errc
ym_sprite_draw_extd(ym_sheet_id sheet_id,
                    ym_sprite_id sprite_id,
                    uint layer,
                    ym_vec2 pos,
                    ym_vec2 scale,
                    float angle)
{
    uint w_width;
    uint w_height;
    ym_gfx_window_get_size(g_window, &w_width, &w_height);

    ym_vec4 real_pos =
    {
        .x = pos.x / w_width * 1.0f,
        .y = pos.y / w_height * -1.0f,
        .z = 0.0f,
        .w = 1.0f,
    };

    ym_mat4 translate = ym_translate_vec4(real_pos);

    const float c = cosf(angle);
    const float s = sinf(angle);
    ym_mat4 rotate =
    {
        .val =
        {
            c,    s,    0.0f, 0.0f,
            -s,   c,    0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
        },
    };

    ym_mat4 scale_mat =
    {
        .val =
        {
            scale.x, 0.0f,    0.0f, 0.0f,
            0.0f,    scale.y, 0.0f, 0.0f,
            0.0f,    0.0f,    1.0f, 0.0f,
            0.0f,    0.0f,    0.0f, 1.0f,
        },
    };

    ym_mat4 tmp = ym_mul_mat4_mat4(rotate, scale_mat);
    ym_mat4 model = ym_mul_mat4_mat4(tmp, translate);
    ym_mat4 res = ym_mul_mat4_mat4(model, ym_view_mat);

    glUniformMatrix4fv(g_ym_render_cfg.uniforms.mvp, 1, GL_FALSE, res.val);

    glBindTexture(GL_TEXTURE_2D, sheet_id);
    glUniform1ui(g_ym_render_cfg.uniforms.texture_id, sprite_id);

    glDrawElements(GL_TRIANGLES, g_ym_render_cfg.ibo_size, GL_UNSIGNED_INT, NULL);

    return ym_errc_success;
}
