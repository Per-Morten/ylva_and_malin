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
        GLubyte* image;
        unsigned int width;
        unsigned int height;

        auto error = lodepng_decode32_file(&image,
                                           &width,
                                           &height,
                                           texture_paths[i]);

        if (error)
        {
            LOG_WARN("Could not decode file: %s, error: %s",
                      texture_paths[i],
                      lodepng_error_text(error));
            return i;
        }

        out_textures[i].width = width;
        out_textures[i].height = height;

        glGenTextures(1, &out_textures[i].id);
        glBindTexture(GL_TEXTURE_2D, out_textures[i].id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, image);

        free(image);

        auto gl_errc = glGetError();
        if (gl_errc != GL_NO_ERROR)
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
