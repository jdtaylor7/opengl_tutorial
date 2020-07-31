#version 330 core

struct DirectionalLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

in vec3 frag_pos;
in vec3 normal_vec;
in vec2 tex_coords;

out vec4 frag_color;

uniform vec3 view_pos;
uniform DirectionalLight light;
uniform Material material;

void main()
{
    // Ambient.
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, tex_coords));

    // Diffuse.
    vec3 norm = normalize(normal_vec);
    vec3 light_dir = normalize(-light.direction);
    float diffuse_magnitude = max(dot(norm, light_dir), 0.0f);
    vec3 diffuse = light.diffuse * diffuse_magnitude * vec3(texture(material.diffuse, tex_coords));

    // Specular.
    vec3 view_dir = normalize(view_pos - frag_pos);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0f), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, tex_coords));

    vec3 result = ambient + diffuse + specular;
    frag_color = vec4(result, 1.0f);
}
