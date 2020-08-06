#ifndef ROOM_HPP
#define ROOM_HPP

#include <filesystem>
#include <vector>

#include "shader.hpp"
#include "shapes.hpp"
#include "utility.hpp"

const std::vector<float> floor_vertices = {
    // positions         // colors          // texture coords
     0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,  // top right
     0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,  // bottom left
    -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f,  // top left
};

const std::vector<unsigned int> floor_indices = {
    0, 1, 3,  // right triangle
    1, 2, 3,  // left triangle
};

class Room
{
public:
    Room(Shader& floor_shader_,
        std::filesystem::path floor_diffuse_texture_path_) :
            floor_shader(floor_shader_),
            floor_diffuse_texture_path(floor_diffuse_texture_path_)
    {
    }

    void init();
    void draw();
private:
    Shader floor_shader;

    std::filesystem::path floor_diffuse_texture_path;

    unsigned int floor_diffuse_texture;

    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;
};

void Room::init()
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * square_vertices.size(), square_vertices.data(), GL_STATIC_DRAW);

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
    floor_shader.set_float("material.texture_diffuse1", floor_diffuse_texture);
}

void Room::draw()
{
    // Draw floor.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, floor_diffuse_texture);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * floor_vertices.size(), floor_vertices.data(), GL_STATIC_DRAW);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

#endif /* ROOM_HPP */
