#version 330 core

struct PointLight
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    // Attenuation parameters.
    float constant;
    float linear;
    float quadratic;
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
uniform PointLight light;
uniform Material material;

void main()
{
    // Ambient.
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, tex_coords));

    // Diffuse.
    vec3 norm = normalize(normal_vec);
    vec3 light_dir = normalize(light.position - frag_pos);
    float diffuse_magnitude = max(dot(norm, light_dir), 0.0f);
    vec3 diffuse = light.diffuse * diffuse_magnitude * vec3(texture(material.diffuse, tex_coords));

    // Specular.
    vec3 view_dir = normalize(view_pos - frag_pos);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0f), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, tex_coords));

    // Attenuation.
    float distance = length(light.position - frag_pos);
    float attenuation = 1.0f / (light.constant + \
                               (light.linear * distance) + \
                               (light.quadratic * distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    vec3 result = ambient + diffuse + specular;
    frag_color = vec4(result, 1.0f);
}
