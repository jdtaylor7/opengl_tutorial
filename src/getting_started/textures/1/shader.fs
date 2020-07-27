#version 330 core

in vec3 vert_color;
in vec2 vert_tex_coords;

out vec4 frag_color;

uniform sampler2D frag_texture;

void main()
{
    frag_color = texture(frag_texture, vert_tex_coords) * vec4(vert_color, 1.0f);
}
