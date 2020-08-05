#ifndef MESH_HPP
#define MESH_HPP

#include <filesystem>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;
};

struct Texture
{
    std::size_t id;
    std::string type;
    std::filesystem::path path;
};

class Mesh
{
public:
    Mesh(std::vector<Vertex>, std::vector<unsigned int>, std::vector<Texture>);

    void init();
    void draw(Shader&);
private:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;
};

Mesh::Mesh(std::vector<Vertex> vertices_,
           std::vector<unsigned int> indices_,
           std::vector<Texture> textures_) :
    vertices(vertices_),
    indices(indices_),
    textures(textures_)
{
}

void Mesh::init()
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

    // Vertex positions.
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // Vertex normals.
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // Vertex textures coordinates.
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));

    glBindVertexArray(0);
}

void Mesh::draw(Shader& shader)
{
    unsigned int diffuse_num = 1;
    unsigned int specular_num = 1;

    for (std::size_t i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);

        std::string num;
        std::string name = textures[i].type;
        if (name == "texture_diffuse")
            num = std::to_string(diffuse_num++);
        else if (name == "texture_specular")
            num = std::to_string(specular_num++);

        shader.set_float(name + num, i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    // Draw mesh.
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}

#endif /* MESH_HPP */
