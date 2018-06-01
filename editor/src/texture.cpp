#include <texture.h>
#include <lodepng.h>
#include <logger.h>

int
create_textures(const char** texture_paths,
                int count,
                texture_t* out_textures)
{
    glActiveTexture(GL_TEXTURE0);
    for (int i = 0; i < count; i++)
    {
        // Load Image
        GLubyte* image;
        auto errc = lodepng_decode32_file(&image,
                                          &out_textures[i].width,
                                          &out_textures[i].height,
                                          texture_paths[i]);

        if (errc)
        {
            LOG_WARN("Could not decode file: %s, error: %s",
                      texture_paths[i],
                      lodepng_error_text(errc));
            return i;
        }

        // Setup OpenGL Texture
        glGenTextures(1, &out_textures[i].id);
        glBindTexture(GL_TEXTURE_2D, out_textures[i].id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, out_textures[i].width, out_textures[i].height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, image);

        free(image);

        if (auto gl_errc = glGetError(); gl_errc != GL_NO_ERROR)
        {
            LOG_WARN("glTexImage2D failed for %s",
                     texture_paths[i]);
            return i;
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }

    return count;
}

void
delete_textures(texture_t* textures,
                int count)
{
    for (int i = 0; i < count; i++)
        glDeleteTextures(1, &textures[i].id);
}
