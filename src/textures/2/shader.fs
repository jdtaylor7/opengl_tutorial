#version 330 core

in vec3 vert_color;
in vec2 vert_tex_coords;

out vec4 frag_color;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mix_val;

void main()
{
    frag_color = mix(texture(texture1, vert_tex_coords),
                     texture(texture2, vert_tex_coords),
                     mix_val);
}
