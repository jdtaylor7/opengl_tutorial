#ifndef ROOM_HPP
#define ROOM_HPP

#include <filesystem>
#include <string>
#include <vector>

#include "lights.hpp"
#include "shader.hpp"
#include "shapes.hpp"
#include "utility.hpp"

const std::vector<float> floor_vertices = scale_square_texture_coords(4.0f);

class Room
{
public:
    Room(Shader* floor_shader_,
        std::filesystem::path floor_diffuse_texture_path_,
        std::filesystem::path floor_specular_texture_path_,
        std::filesystem::path floor_normal_texture_path_,
        SceneLighting* scene_lighting_) :
            floor_shader(floor_shader_),
            floor_diffuse_texture_path(floor_diffuse_texture_path_),
            floor_specular_texture_path(floor_specular_texture_path_),
            floor_normal_texture_path(floor_normal_texture_path_),
            sl(scene_lighting_)
    {
    }

    void init();
    void deinit();
    void draw();
private:
    Shader* floor_shader;

    std::filesystem::path floor_diffuse_texture_path;
    std::filesystem::path floor_specular_texture_path;
    std::filesystem::path floor_normal_texture_path;

    unsigned int floor_diffuse_texture;
    unsigned int floor_specular_texture;
    unsigned int floor_normal_texture;

    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;

    SceneLighting* sl;
};

void Room::init()
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * floor_vertices.size(), floor_vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * square_indices.size(), square_indices.data(), GL_STATIC_DRAW);

    // Vertex positions.
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    // Vertex normals.
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    // Vertex textures coordinates.
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glBindVertexArray(0);

    // Load textures.
    floor_diffuse_texture = load_texture_from_file(floor_diffuse_texture_path);
    floor_specular_texture = load_texture_from_file(floor_specular_texture_path);
    floor_normal_texture = load_texture_from_file(floor_normal_texture_path);
}

void Room::deinit()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void Room::draw()
{
    /*
     * Draw floor.
     */
    // Set shader attributes.
    if (sl)
    {
        floor_shader->use();
        
        // Directional light properties.
        if (sl->dir)
        {
            floor_shader->set_vec3("dir_light.direction", sl->dir->direction);

            floor_shader->set_vec3("dir_light.ambient", sl->dir->ambient);
            floor_shader->set_vec3("dir_light.diffuse", sl->dir->diffuse);
            floor_shader->set_vec3("dir_light.specular", sl->dir->specular);
        }
        else
        {
            std::cerr << "Room::draw: SceneLighting->DirectionalLight pointer is null.\n";
        }

        // Point light properties.
        for (std::size_t i = 0; i < sl->points.size(); i++)
        {
            std::string attr_prefix{"point_lights[" + std::to_string(i) + "]."};

            if (sl->points[i])
            {
                floor_shader->set_vec3(attr_prefix + "position", sl->points[i]->position);
                floor_shader->set_vec3(attr_prefix + "ambient", sl->points[i]->ambient);
                floor_shader->set_vec3(attr_prefix + "diffuse", sl->points[i]->color * sl->points[i]->diffuse);
                floor_shader->set_vec3(attr_prefix + "specular", sl->points[i]->color * sl->points[i]->specular);
                floor_shader->set_float(attr_prefix + "constant", sl->points[i]->constant);
                floor_shader->set_float(attr_prefix + "linear", sl->points[i]->linear);
                floor_shader->set_float(attr_prefix + "quadratic", sl->points[i]->quadratic);
            }
            else
            {
                std::cerr << "Room::draw: SceneLighting->PointLight pointer is null.\n";
            }
        }

        // Spotlight properties.
        if (sl->spot)
        {
            floor_shader->set_vec3("spotlight.position", sl->spot->position);
            floor_shader->set_vec3("spotlight.direction", sl->spot->direction);

            floor_shader->set_float("spotlight.inner_cutoff", glm::cos(glm::radians(sl->spot->inner_cutoff)));
            floor_shader->set_float("spotlight.outer_cutoff", glm::cos(glm::radians(sl->spot->outer_cutoff)));

            floor_shader->set_vec3("spotlight.ambient", sl->spot->ambient);
            floor_shader->set_vec3("spotlight.diffuse", sl->spot->diffuse);
            floor_shader->set_vec3("spotlight.specular", sl->spot->specular);

            floor_shader->set_float("spotlight.constant", sl->spot->constant);
            floor_shader->set_float("spotlight.linear", sl->spot->linear);
            floor_shader->set_float("spotlight.quadratic", sl->spot->quadratic);
        }
        else
        {
            std::cerr << "Room::draw: SceneLighting->Spotlight pointer is null.\n";
        }

        // // Material properties.
        // floor_shader->set_float("material.shininess", 32.0f);
    }
    else
    {
        std::cerr << "Room::draw: SceneLighting pointer is null.\n";
    }

    // Set textures.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, floor_diffuse_texture);
    floor_shader->set_float("material.texture_diffuse1", floor_diffuse_texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, floor_specular_texture);
    floor_shader->set_float("material.texture_specular1", floor_specular_texture);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glDrawElements(GL_TRIANGLES, square_indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

#endif /* ROOM_HPP */
