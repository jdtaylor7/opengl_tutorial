#include <iostream>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.hpp"

constexpr std::size_t SCREEN_WIDTH = 800;
constexpr std::size_t SCREEN_HEIGHT = 600;

const std::vector<float> vertices = {
    // positions         // colors
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  // top
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main()
{
    unsigned int VAO;
    unsigned int VBO;

    /*
     * GLFW initialization and configuration.
     */
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /*
     * GLFW window creation.
     */
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello Triangle", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    /*
     * Load OpenGL function pointers.
     */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    /*
     * Build buffers and vertex array object.
     */

    // Generate and bind a "vertex array object" (VAO) to store the VBO and
    // corresponding vertex attribute configurations.
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Send vertex data to the vertex shader. Do so by allocating GPU
    // memory, which is managed by "vertex buffer objects" (VBOs).
    glGenBuffers(1, &VBO);

    // Bind VBO to the vertex buffer object, GL_ARRAY_BUFFER. Buffer
    // operations on GL_ARRAY_BUFFER then apply to VBO.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    // Specify vertex data format.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Can now unbind VAO and VBO, will rebind VAO as necessary in render loop.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    /*
     * Create shader program.
     */
    Shader shader("src/shaders3/shader.vs", "src/shaders3/shader.fs");

    /*
     * Render loop.
     */
    while (!glfwWindowShouldClose(window))
    {
        /*
         * Input.
         */
        process_input(window);

        /*
         * Render.
         */

        // Color buffer.
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();

        // Render triangle.
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        /*
         * Swap buffers and poll I/O events.
         */
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    /*
     * Clean up.
     */
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}
