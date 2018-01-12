#include <ym_gfx_gl.h>
#include <ym_gfx_sprite.h>
#include <ym_allocator.h>

#include <stdio.h>


static
const char*
gl_error_str(GLenum e)
{
    switch (e)
    {
        case GL_NO_ERROR:
            return "GL_NO_ERROR";
        case GL_INVALID_ENUM:
            return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE:
            return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION:
            return "GL_INVALID_OPERATION";
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "GL_INVALID_FRAMEBUFFER_OPERATION";
        case GL_OUT_OF_MEMORY:
            return "GL_OUT_OF_MEMORY";
        case GL_STACK_UNDERFLOW:
            return "GL_STACK_UNDERFLOW";
        case GL_STACK_OVERFLOW:
            return "GL_STACK_OVERFLOW";
    }

    return "Unknown Error!";
}

static ym_mem_region* ym_gfx_gl_mem_reg;

ym_errc
ym_gfx_gl_init(ym_mem_region* region)
{
    ym_errc errc = ym_gfx_gl_load_procs();

    ym_gfx_gl_mem_reg = region;

    return errc;
}

ym_errc
ym_gfx_gl_create_shader(const char* file_path,
                        GLenum type,
                        GLuint* out_shader) // Also take in memory?
{
    YM_ASSERT(file_path && out_shader,
              ym_errc_invalid_input,
              "file_path and out_shader must not be NULL");

    ym_allocator allocator;
    ym_create_allocator(ym_gfx_gl_mem_reg, ym_alloc_strategy_linear, 1048, &allocator);


    /// \todo Use ym_mem rather than malloc and free.
    /// Can probably just use a linear allocator
    FILE* file = fopen(file_path, "r");
    if (!file)
    {
        YM_WARN("%s:, Could not open file: %s",
                ym_errc_str(ym_errc_system_error),
                file_path);
        return ym_errc_system_error;
    }

    fseek(file, 0L, SEEK_END);
    long file_len = ftell(file);
    rewind(file);

    GLchar* source;
    ym_allocate(&allocator, file_len * sizeof(GLchar) + 1, &source);
    //GLchar* source = malloc(file_len * sizeof(GLchar) + 1);
    // Ensure terminating \0.
    source[file_len * sizeof(GLchar)] = '\0';
    if (!fread(source, sizeof(GLchar), file_len, file))
    {
        YM_WARN("%s:, Could not read from file: %s",
                ym_errc_str(ym_errc_system_error),
                file_path);

        //free(source);
        fclose(file);
        return ym_errc_system_error;

    }
    fclose(file);

    GLuint shader = glCreateShader(type);
    if (!shader)
    {
        YM_WARN("%s: Could not create shader: %s",
                ym_errc_str(ym_errc_gl_error),
                gl_error_str(glGetError()));

        //free(source);
        return ym_errc_gl_error;
    }

    glShaderSource(shader, 1, (const GLchar**)&source, NULL);

    //free(source);

    glCompileShader(shader);

    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (compiled == GL_FALSE)
    {
        GLint log_len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);

        GLchar* log_buf = malloc(log_len * sizeof(GLchar));

        GLsizei len;
        glGetShaderInfoLog(shader, log_len,
                           &len, log_buf);

        YM_WARN("%s: Compilation failed: %s\n%s",
                ym_errc_str(ym_errc_gl_error),
                file_path,
                log_buf);

        free(log_buf);

        return ym_errc_gl_error;
    }

    ym_deallocate(&allocator, 1048, source);
    ym_destroy_allocator(ym_gfx_gl_mem_reg, &allocator);



    *out_shader = shader;
    return ym_errc_success;
}

ym_errc
ym_gfx_gl_create_program(GLuint* shaders,
                         int shader_count,
                         GLuint* out_program)
{
    /// \todo Use ym_mem rather than malloc and free.
    YM_ASSERT(shaders && out_program && shader_count > 0,
              ym_errc_invalid_input,
              "shaders and out_program cannot be NULL"
              "and shader_count must be > 0");

    GLuint program = glCreateProgram();
    if (!program)
    {
        YM_WARN("%s: Could not create program: %s",
                ym_errc_str(ym_errc_gl_error),
                gl_error_str(glGetError()));
        return ym_errc_gl_error;
    }

    for (int i = 0; i < shader_count; i++)
    {
        glAttachShader(program, shaders[i]);

        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
        {
            YM_WARN("%s: Could not attach shader: %s",
                    ym_errc_str(ym_errc_gl_error),
                    gl_error_str(err));
            return ym_errc_gl_error;
        }
    }

    glLinkProgram(program);

    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);

    if (linked != GL_TRUE)
    {
        GLint log_len = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_len);

        GLchar* log_buf = malloc(log_len * sizeof(GLchar));

        GLsizei len;
        glGetProgramInfoLog(program, log_len,
                            &len, log_buf);

        YM_WARN("%s: Linking failed:\n%s",
                ym_errc_str(ym_errc_gl_error),
                log_buf);

        free(log_buf);

        return ym_errc_gl_error;
    }

    *out_program = program;

    return ym_errc_success;
}

ym_errc
ym_gfx_gl_create_texture(const char* file_path,
                         GLenum texture_slot,
                         GLuint* out_texture)
{
    /// \todo Use ym_mem rather than malloc and free.
    /// Can also probably just use a stack allocator, and think of which way I free.
    GLubyte* image;
    GLsizei width;
    GLsizei height;
    ym_errc errc = ym_gfx_load_png(file_path, &image,
                                   &width, &height);

    if (errc != ym_errc_success)
    {
        YM_WARN("%s: Could not load image: %s",
                ym_errc_str(errc),
                file_path);
        return errc;
    }

    // Flip image
    int width_in_bytes = width * 4;
    for (int row = 0; row < height / 2; row++)
    {
        GLubyte* top = image + row * width_in_bytes;
        GLubyte* bottom = image + (height - row - 1) * width_in_bytes;
        for (int col = 0; col < width_in_bytes; col++)
        {
            GLubyte tmp = *top;
            *top = *bottom;
            *bottom = tmp;
            top++;
            bottom++;
        }
    }

    glActiveTexture(texture_slot);
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        YM_WARN("%s: Could not set active texture for %s: %s",
                ym_errc_str(ym_errc_gl_error),
                file_path,
                gl_error_str(err));
        free(image);
        return ym_errc_gl_error;
    }

    glGenTextures(1, out_texture);
    glBindTexture(GL_TEXTURE_2D, (*out_texture));
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image);
    free(image);

    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        YM_WARN("%s: glTexImage2D failed for %s: %s",
                ym_errc_str(ym_errc_gl_error),
                file_path,
                gl_error_str(err));
        return ym_errc_gl_error;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return ym_errc_success;
}

ym_errc
ym_gfx_gl_get_uniform(GLuint program,
                      const char* uniform_name,
                      GLint* out_uniform)
{
    *out_uniform = glGetUniformLocation(program, uniform_name);
    if (*out_uniform == -1)
    {
        YM_WARN("%s: Could not get location of uniform %s: %s",
                ym_errc_str(ym_errc_gl_error),
                uniform_name,
                gl_error_str(glGetError()));
        return ym_errc_gl_error;
    }

    return ym_errc_success;
}
