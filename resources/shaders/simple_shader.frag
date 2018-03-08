#version 410

// Specify location because then I can call it whatever I want
in vec2 tex_coords;

uniform vec4 u_color;
uniform sampler2D basic_texture;

out vec4 out_frag_colour;

void main()
{
    vec4 texel = texture(basic_texture, tex_coords);
    out_frag_colour = texel;
}
