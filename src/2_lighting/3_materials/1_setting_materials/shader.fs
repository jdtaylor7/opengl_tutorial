#version 330 core

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

in vec3 frag_pos;
in vec3 normal_vec;

out vec4 frag_color;

uniform vec3 object_color;
uniform vec3 light_color;
uniform vec3 light_pos;
uniform vec3 view_pos;
uniform Material material;

void main()
{
    // Ambient.
    vec3 ambient = light_color * material.ambient;

    // Diffuse.
    vec3 norm = normalize(normal_vec);
    vec3 light_dir = normalize(light_pos - frag_pos);
    float diffuse_magnitude = max(dot(norm, light_dir), 0.0f);
    vec3 diffuse = light_color * (diffuse_magnitude * material.diffuse);

    // Specular.
    vec3 view_dir = normalize(view_pos - frag_pos);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0f), material.shininess);
    vec3 specular = light_color * (spec * material.specular);

    vec3 result = (ambient + diffuse + specular) * object_color;
    frag_color = vec4(result, 1.0f);
}
