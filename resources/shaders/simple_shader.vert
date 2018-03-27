#version 410

layout(location = 0) in vec2 in_vp;
layout(location = 1) in vec2 in_vt;

out vec2 tex_coords;

uniform uint u_atlas_col_count = 3;
uniform uint u_atlas_row_count = 4;
uniform mat4 u_matrix;
uniform uint u_texture_id = 0;

void main()
{
    vec2 scaled = vec2(in_vt.x / u_atlas_col_count,
                       in_vt.y / u_atlas_row_count);

    float col = u_texture_id % u_atlas_col_count;

    // u_atlas_row_count - 1 to count texture_id from top left, rather than bottom left.
    float row = u_atlas_row_count - 1 - u_texture_id / u_atlas_col_count;

    vec2 offset = vec2(col / u_atlas_col_count,
                       row / u_atlas_row_count);

    tex_coords = scaled + offset;

    gl_Position = u_matrix * vec4(in_vp, 0.0, 1.0);
}
