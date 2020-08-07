#version 330 core

struct DirectionalLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

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

struct Spotlight
{
    vec3 position;
    vec3 direction;
    float inner_cutoff;
    float outer_cutoff;

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
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};

#define NUM_POINT_LIGHTS 1

in vec3 frag_pos;
in vec3 normal_vec;
in vec2 tex_coords;
in vec4 frag_pos_light_space;

uniform vec3 view_pos;
uniform DirectionalLight dir_light;
uniform PointLight point_lights[NUM_POINT_LIGHTS];
uniform Spotlight spotlight;
uniform Material material;
uniform sampler2D shadow_map;

out vec4 frag_color;

float calc_shadow(vec4 frag_pos)
{
    // Normalize perspective.
    vec3 proj_coords = frag_pos.xyz / frag_pos.w;

    // Transform from [-1, 1] to [0, 1].
    proj_coords = (proj_coords * 0.5f) + 0.5f;

    // Compute closest and current depths.
    float closest_depth = texture(shadow_map, proj_coords.xy).r;
    float current_depth = proj_coords.z;

    // Check if current frag pos in shadow.
    float shadow = current_depth > closest_depth ? 1.0f : 0.0f;

    return shadow;
}

vec3 calc_dir_light(DirectionalLight light, vec3 normal, vec3 view_dir)
{
    // Ambient.
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, tex_coords));

    // Diffuse.
    vec3 light_dir = normalize(-light.direction);
    float diff = max(dot(normal, light_dir), 0.0f);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, tex_coords));

    // Specular.
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0f), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, tex_coords));

    return (ambient + diffuse + specular);
}

vec3 calc_point_light(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir)
{
    // Ambient.
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, tex_coords));

    // Diffuse.
    vec3 light_dir = normalize(light.position - frag_pos);
    float diff = max(dot(normal, light_dir), 0.0f);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, tex_coords));

    // Specular.
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0f), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, tex_coords));

    // Attenuation.
    float distance = length(light.position - frag_pos);
    float attenuation = 1.0f / (light.constant + \
                               (light.linear * distance) + \
                               (light.quadratic * distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    // Shadow.
    float shadow = calc_shadow(frag_pos_light_space);

    return (ambient + (1.0f - shadow) * (diffuse + specular));
}

vec3 calc_spotlight(Spotlight light, vec3 normal, vec3 frag_pos, vec3 view_dir)
{
    // Ambient.
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, tex_coords));

    // Diffuse.
    vec3 light_dir = normalize(light.position - frag_pos);
    float diff = max(dot(normal, light_dir), 0.0f);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, tex_coords));

    // Specular.
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0f), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, tex_coords));

    // Attenuation.
    float distance = length(light.position - frag_pos);
    float attenuation = 1.0f / (light.constant + \
                               (light.linear * distance) + \
                               (light.quadratic * distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    // Compute spotlight cone.
    float theta = dot(light_dir, normalize(-light.direction));
    float epsilon = light.inner_cutoff - light.outer_cutoff;
    float intensity = clamp((theta - light.outer_cutoff) / epsilon, 0.0f, 1.0f);

    diffuse *= intensity;
    specular *= intensity;

    return (ambient + diffuse + specular);
}

void main()
{
    // Precomputed values for light calcs.
    vec3 normal = normalize(normal_vec);
    vec3 view_dir = normalize(view_pos - frag_pos);

    vec3 result = vec3(0.0f);

    // Directional light.
    // result += calc_dir_light(dir_light, normal, view_dir);

    // Point lights.
    for (int i = 0; i < NUM_POINT_LIGHTS; i++)
        result += calc_point_light(point_lights[i], normal, frag_pos, view_dir);

    // Spotlight.
    // result += calc_spotlight(spotlight, normal, frag_pos, view_dir);

    frag_color = vec4(result, 1.0f);
}
