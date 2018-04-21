#include <ym_gfx_sprite.h>
#include <lodepng.h>

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
        GLint mvp;
        GLint texture_id;
        GLint atlas_col_count;
        GLint atlas_row_count;
    } uniforms;

    ym_mat4 view_matrix;
    ym_mat4 projection_matrix;

} g_ym_render_cfg;


/////////////////////////////////////////////////////////////////////
/// Sprite sheet "container interaction"
/////////////////////////////////////////////////////////////////////
typedef struct
{
    uint row_count;
    uint col_count;
    GLuint texture_id;
} sprite_sheet;

#define MAX_SPRITE_SHEETS 256
static sprite_sheet g_sprite_sheets[MAX_SPRITE_SHEETS];
static int g_sprite_sheets_count = 0;

YM_INLINE static
sprite_sheet*
get_sheet_(ym_sheet_id sheet_id)
{
    return &g_sprite_sheets[sheet_id];
}

ym_errc
add_sheet_(uint row_count, uint col_count, GLuint texture_id, ym_sheet_id* out_id)
{
    if (g_sprite_sheets_count + 1 > MAX_SPRITE_SHEETS)
    {
        return ym_errc_container_full;
    }

    int my_id = g_sprite_sheets_count++;
    g_sprite_sheets[my_id].row_count = row_count;
    g_sprite_sheets[my_id].col_count = col_count;
    g_sprite_sheets[my_id].texture_id = texture_id;
    *out_id = my_id;

    return ym_errc_success;
}

sprite_sheet*
get_sheets_(int* out_count)
{
    *out_count = g_sprite_sheets_count;
    return g_sprite_sheets;
}

/////////////////////////////////////////////////////////////////////
/// PNG loading
/////////////////////////////////////////////////////////////////////
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


/////////////////////////////////////////////////////////////////////
/// ym_sprite API implementation
/////////////////////////////////////////////////////////////////////
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
        -0.5f,  0.5f,
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,
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

    // Setup projection matrix
    uint w_width;
    uint w_height;
    ym_gfx_window_get_size(g_window, &w_width, &w_height);
    g_ym_render_cfg.projection_matrix = ym_project(0.5f, 100.0f, 90.0f, (float)w_width / (float)w_height);

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
    GLuint texture_id = 0;
    ym_errc errc = ym_gfx_gl_create_texture(filename,
                                            GL_TEXTURE0,
                                            &texture_id);

    if (errc != ym_errc_success)
        return errc;

    errc = add_sheet_(row_count, col_count, texture_id, out_sheet_id);
    return errc;
}

ym_errc
ym_sprite_delete_sheet(ym_sheet_id sheet_id)
{
    sprite_sheet* sheet = get_sheet_(sheet_id);
    glDeleteTextures(1, &sheet->texture_id);

    return ym_errc_success;
}

ym_errc
ym_sprite_draw(ym_sheet_id sheet_id,
               ym_sprite_id sprite_id,
               uint layer,
               ym_vec2 pos)
{
    ym_vec4 real_pos =
    {
        .x = pos.x,
        .y = -pos.y,
        .z = 0.0f,
        .w = 1.0f,
    };

    ym_mat4 translated = ym_translate_vec4(real_pos);
    ym_mat4 viewed = ym_mul_mat4_mat4(translated, g_ym_render_cfg.view_matrix);
    ym_mat4 projected = ym_mul_mat4_mat4(viewed, g_ym_render_cfg.projection_matrix);

    glUniformMatrix4fv(g_ym_render_cfg.uniforms.mvp, 1, GL_FALSE, projected.val);


    sprite_sheet* sheet = get_sheet_(sheet_id);
    glBindTexture(GL_TEXTURE_2D, sheet->texture_id);
    glUniform1ui(g_ym_render_cfg.uniforms.texture_id, sprite_id);
    glUniform1ui(g_ym_render_cfg.uniforms.atlas_row_count, sheet->row_count);
    glUniform1ui(g_ym_render_cfg.uniforms.atlas_col_count, sheet->col_count);


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
    ym_vec4 real_pos =
    {
        .x = pos.x,
        .y = -pos.y,
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

    ym_mat4 scaled = ym_mul_mat4_mat4(rotate, scale_mat);
    ym_mat4 translated = ym_mul_mat4_mat4(scaled, translate);
    ym_mat4 viewed = ym_mul_mat4_mat4(translated, g_ym_render_cfg.view_matrix);
    ym_mat4 projected = ym_mul_mat4_mat4(viewed, g_ym_render_cfg.projection_matrix);

    glUniformMatrix4fv(g_ym_render_cfg.uniforms.mvp, 1, GL_FALSE, projected.val);

    sprite_sheet* sheet = get_sheet_(sheet_id);
    glBindTexture(GL_TEXTURE_2D, sheet->texture_id);
    glUniform1ui(g_ym_render_cfg.uniforms.texture_id, sprite_id);
    glUniform1ui(g_ym_render_cfg.uniforms.atlas_row_count, sheet->row_count);
    glUniform1ui(g_ym_render_cfg.uniforms.atlas_col_count, sheet->col_count);

    glDrawElements(GL_TRIANGLES, g_ym_render_cfg.ibo_size, GL_UNSIGNED_INT, NULL);

    return ym_errc_success;
}

void
ym_sprite_set_camera_pos(ym_vec3 camera_pos)
{
    ym_vec4 camera = {.x = camera_pos.x, .y = camera_pos.y, .z = camera_pos.z, .w = 1.0f};
    ym_vec4 target = {.x = camera_pos.x, .y = camera_pos.y, .z = 0.0f, .w = 1.0f};
    ym_vec4 up_dir = {0.0f, 1.0f, 0.0f, 1.0f};
    g_ym_render_cfg.view_matrix = ym_lookat(camera, target, up_dir);
}
