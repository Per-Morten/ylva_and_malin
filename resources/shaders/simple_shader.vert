#version 410

layout(location = 0) in vec3 in_vp;
layout(location = 1) in vec2 in_vt;

out vec2 tex_coords;

uniform mat4 u_matrix;

void main()
{
    tex_coords = in_vt;
    gl_Position = u_matrix * vec4(in_vp, 1.0);
}
