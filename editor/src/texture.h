#pragma once
#include <GL/GLEW.h>
#include <gl/GLU.h>

struct texture_t
{
    GLuint id{};
    unsigned int width;
    unsigned int height;
};

int
create_textures(const char** texture_paths,
                int count,
                texture_t* out_textures);

void
delete_textures(texture_t* textures,
                int count);
