#ifndef POINT_LIGHT_HPP
#define POINT_LIGHT_HPP

#include <glm/glm.hpp>

#include "shapes.hpp"

class PointLight
{
public:
    PointLight(glm::vec3 position_, glm::vec3 color_, float scale_factor_) :
        position(position_),
        color(color_),
        scale_factor(scale_factor_)
    {
    }

    void init();
    void draw();

    glm::vec3 get_position() const { return position; };
    glm::vec3 get_color() const { return color; };
    float get_scale_factor() const { return scale_factor; };
private:
    glm::vec3 position;
    glm::vec3 color;
    float scale_factor;

    unsigned int vao;
    unsigned int vbo;
};

void PointLight::init()
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * cube_vertices.size(), cube_vertices.data(), GL_STATIC_DRAW);

    // Specify vertex data format.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void PointLight::draw()
{
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

#endif /* POINT_LIGHT_HPP */
