#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

constexpr std::size_t SCREEN_WIDTH = 800;
constexpr std::size_t SCREEN_HEIGHT = 600;

const std::vector<float> vertices = {
     0.5f,  0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
    -0.5f,  0.5f, 0.0f
};

const std::vector<unsigned int> indices = {
    0, 1, 3,  // first triangle
    1, 2, 3   // second triangle
};

/*
 * Vertex shader.
 */
const char* vertex_shader_source = "#version 330 core\n"
    "layout (location = 0) in vec3 a_pos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(a_pos.x, a_pos.y, a_pos.z, 1.0);\n"
    "}\0";

/*
 * Fragment shader.
 */
const char* fragment_shader_source = "#version 330 core\n"
    "out vec4 frag_color;\n"
    "void main()\n"
    "{\n"
    "   frag_color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";

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
    unsigned int EBO;
    unsigned int vertex_shader;
    unsigned int fragment_shader;
    unsigned int shader_program;
    int success;
    char info_log[512];

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
    // memory, which is managed by a "vertex buffer object" (VBO). Also send
    // index data to the vertex shader via an "element buffer object" (EBO).
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind VBO to the vertex buffer object, GL_ARRAY_BUFFER. Buffer
    // operations on GL_ARRAY_BUFFER then apply to VBO.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    // Bind EBO to the element buffer object, GL_ELEMENT_ARRAY_BUFFER. Buffer
    // operations on GL_ELEMENT_ARRAY_BUFFER then apply to EBO.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

    // Specify vertex data format.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Can now unbind VAO and VBO, will rebind VAO as necessary in render loop.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    /*
     * Build shaders.
     */

    // Create vertex shader object.
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);

    // Check for vertex shader compilation errors.
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << '\n';
    }

    // Create fragment shader object.
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);

    // Check for fragment shader compilation errors.
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << '\n';
    }

    // Build shader program.
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    // Check for shader program link errors.
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        std::cout << "ERROR::PROGRAM::LINKING_FAILED\n" << info_log << '\n';
    }

    // Delete the shader objects since they've already been linked into the
    // shader program.
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    // Draw in polygon mode.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    /*
     * Render loop.
     */
    while (!glfwWindowShouldClose(window))
    {
        // Input.
        process_input(window);

        // Render.
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shader_program);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Swap buffers and poll I/O events.
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    /*
     * Clean up.
     */
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}
