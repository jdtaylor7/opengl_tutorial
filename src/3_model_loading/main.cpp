#include <cassert>
#include <iostream>
#include <filesystem>
#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "mesh.hpp"
#include "model.hpp"
#include "lights.hpp"
#include "room.hpp"
#include "shader.hpp"

constexpr std::size_t SCREEN_WIDTH = 800;
constexpr std::size_t SCREEN_HEIGHT = 600;

ModelSettings backpack("backpack", true, 0.5f);
ModelSettings drone("drone", false, 0.002f);

ModelSettings model_settings = drone;
bool show_mesh = false;

float room_scale_factor = 4.0f;

namespace fs = std::filesystem;
const fs::path shader_path = "src/3_model_loading";
const fs::path texture_path = "assets/textures";

const fs::path plight_vshader_path = shader_path / "point_light.vs";
const fs::path plight_fshader_path = shader_path / "point_light.fs";
const fs::path model_vshader_path = shader_path / "model.vs";
const fs::path model_fshader_path = shader_path / "model.fs";
const fs::path floor_vshader_path = shader_path / "floor.vs";
const fs::path floor_fshader_path = shader_path / "floor.fs";

const fs::path model_directory = "assets/models/" + model_settings.name;
const fs::path model_obj_path = model_directory / (model_settings.name + ".obj");

const fs::path floor_diffuse_path = texture_path / "stone_floor/diffuse.png";
const fs::path floor_specular_path = texture_path / "stone_floor/specular.png";
const fs::path floor_normal_path = texture_path / "stone_floor/normal.png";

glm::vec3 camera_pos = glm::vec3(4.91f, 1.62f, 5.61f);
glm::vec3 camera_front = glm::vec3(-0.67f, -0.30f, -0.68f);
glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);

float delta_time = 0.0f;
float last_frame = 0.0f;

float camera_speed = 2.5f * delta_time;

float lastx = SCREEN_WIDTH / 2;
float lasty = SCREEN_HEIGHT / 2;

constexpr float mouse_sensitivity = 0.05f;

float yaw = -135.0f;
float pitch = -17.5f;

bool first_mouse = true;

float fov = 45.0f;

const glm::vec3 light_pos(1.2f, 1.0f, 2.0f);

const glm::vec3 dir_light_color = glm::vec3(1.0f);

const std::vector<glm::vec3> point_light_colors{
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 1.0f),
    glm::vec3(1.0f, 1.0f, 1.0f),
};

const std::vector<glm::vec3> point_light_positions = {
    glm::vec3( 0.7f,  0.2f,  2.0f),
    glm::vec3( 2.3f, -1.0f, -1.5f),
    glm::vec3(-4.0f,  2.0f, 2.0f),
    glm::vec3( 0.0f,  0.0f, -3.0f),
};

const float point_light_scale_factor = 0.2f;

const glm::vec3 spotlight_color = glm::vec3(1.0f);

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera_speed = 5.0f * delta_time;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        camera_speed = 2.5f * delta_time;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera_pos += camera_speed * camera_front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera_pos -= camera_speed * camera_front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera_pos -= glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera_pos += glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera_pos += camera_speed * camera_up;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera_pos -= camera_speed * camera_up;

    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
    {
        std::cout << "camera_pos: " << camera_pos.x << ", " << camera_pos.y << ", " << camera_pos.z << '\n';
        std::cout << "camera_front: " << camera_front.x << ", " << camera_front.y << ", " << camera_front.z << '\n';
        std::cout << "yaw: " << yaw << '\n';
        std::cout << "pitch: " << pitch << '\n';
        std::cout << '\n';
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (first_mouse)
    {
        lastx = xpos;
        lasty = ypos;
        first_mouse = false;
    }

    float xoffset = xpos - lastx;
    float yoffset = lasty - ypos;
    lastx = xpos;
    lasty = ypos;

    xoffset *= mouse_sensitivity;
    yoffset *= mouse_sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camera_front = glm::normalize(direction);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= yoffset;

    if (fov <= 1.0f)
        fov = 1.0f;
    else if (fov >= 45.0f)
        fov = 45.0f;
}

int main()
{
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    /*
     * Load OpenGL function pointers.
     */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    /*
     * Set global OpenGL state.
     */
    glEnable(GL_DEPTH_TEST);
    if (show_mesh)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    /*
     * Create shader programs.
     */
    Shader plight_shader(plight_vshader_path.string(), plight_fshader_path.string());
    Shader model_shader(model_vshader_path.string(), model_fshader_path.string());
    Shader floor_shader(floor_vshader_path.string(), floor_fshader_path.string());

    /*
     * Initialize model.
     */
    Model model_object(model_obj_path, model_settings.flip_textures);
    model_object.init();

    /*
     * Initialize room.
     */
    Room room(floor_shader,
        floor_diffuse_path,
        floor_specular_path,
        floor_normal_path);
    room.init();

    /*
     * Initialize point lights.
     */
    assert(point_light_positions.size() == point_light_colors.size());
    std::vector<PointLight> point_lights;
    for (std::size_t i = 0; i < point_light_positions.size(); i++)
    {
        PointLight point_light(point_light_positions[i],
            point_light_colors[i],
            point_light_scale_factor);
        point_light.init();
        point_lights.push_back(point_light);
    }

    /*
     * Render loop.
     */
    while (!glfwWindowShouldClose(window))
    {
        float current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        /*
         * Input.
         */
        process_input(window);

        /*
         * Render.
         */
        // Color buffer.
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Initial MVP matrix definitions.
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        /*
         * Draw backpack.
         */
        model_shader.use();

        // Set MVP matrices.
        model = glm::mat4(1.0f);
        view = glm::mat4(1.0f);
        projection = glm::mat4(1.0f);

        view = glm::lookAt(camera_pos, camera_pos + camera_front, camera_up);
        projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);
        model = glm::translate(model, glm::vec3(0.0f));
        model = glm::scale(model, glm::vec3(model_settings.scale_factor));

        // Position properties.
        model_shader.set_vec3("view_pos", camera_pos);

        // Directional light properties.
        // TODO remove lighting updates from here, put in model class
        model_shader.set_vec3("dir_light.direction", glm::vec3(0.0f, -1.0f, 0.0f));

        model_shader.set_vec3("dir_light.ambient", glm::vec3(0.2f));
        model_shader.set_vec3("dir_light.diffuse", glm::vec3(0.5f));
        model_shader.set_vec3("dir_light.specular", glm::vec3(1.0f));

        // Point light properties.
        for (std::size_t i = 0; i < point_light_positions.size(); i++)
        {
            std::string attr_prefix{"point_lights[" + std::to_string(i) + "]."};

            model_shader.set_vec3(attr_prefix + "position", point_light_positions[i]);
            model_shader.set_vec3(attr_prefix + "ambient", glm::vec3(0.05f));
            model_shader.set_vec3(attr_prefix + "diffuse", point_light_colors[i] * glm::vec3(0.5f));
            model_shader.set_vec3(attr_prefix + "specular", point_light_colors[i] * glm::vec3(1.0f));
            model_shader.set_float(attr_prefix + "constant", 1.0f);
            model_shader.set_float(attr_prefix + "linear", 0.07f);
            model_shader.set_float(attr_prefix + "quadratic", 0.017f);
        }

        // Spotlight properties.
        model_shader.set_vec3("spotlight.position", camera_pos);
        model_shader.set_vec3("spotlight.direction", camera_front);
        model_shader.set_float("spotlight.inner_cutoff", glm::cos(glm::radians(12.5f)));
        model_shader.set_float("spotlight.outer_cutoff", glm::cos(glm::radians(17.5f)));

        model_shader.set_vec3("spotlight.ambient", glm::vec3(0.0f));
        model_shader.set_vec3("spotlight.diffuse", glm::vec3(0.5f));
        model_shader.set_vec3("spotlight.specular", glm::vec3(1.0f));

        model_shader.set_float("spotlight.constant", 1.0f);
        model_shader.set_float("spotlight.linear", 0.07f);
        model_shader.set_float("spotlight.quadratic", 0.017f);

        // Material properties.
        model_shader.set_float("material.shininess", 32.0f);

        // Render backpack.
        model_shader.set_mat4fv("projection", projection);
        model_shader.set_mat4fv("view", view);
        model_shader.set_mat4fv("model", model);

        model_object.draw(model_shader);

        /*
         * Draw point lights.
         */
        // Set MVP matrices.
        model = glm::mat4(1.0f);
        view = glm::mat4(1.0f);
        projection = glm::mat4(1.0f);
        view = glm::lookAt(camera_pos, camera_pos + camera_front, camera_up);
        projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);

        // Set light shader values.
        plight_shader.use();

        // Set MVP matrices.
        plight_shader.set_mat4fv("projection", projection);
        plight_shader.set_mat4fv("view", view);

        // Render point light.
        for (auto& point_light : point_lights)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, point_light.position);
            model = glm::scale(model, glm::vec3(point_light.scale_factor));
            plight_shader.set_mat4fv("model", model);
            plight_shader.set_vec3("color", point_light.color);
            point_light.draw();
        }

        /*
         * Draw floor.
         */
        floor_shader.use();

        // Set MVP matrices.
        model = glm::mat4(1.0f);
        view = glm::mat4(1.0f);
        projection = glm::mat4(1.0f);

        view = glm::lookAt(camera_pos, camera_pos + camera_front, camera_up);
        projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);
        model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(room_scale_factor));

        // Position properties.
        floor_shader.set_vec3("view_pos", camera_pos);

        // Directional light properties.
        // TODO remove lighting updates from here, put in room class
        floor_shader.set_vec3("dir_light.direction", glm::vec3(0.0f, -1.0f, 0.0f));

        floor_shader.set_vec3("dir_light.ambient", glm::vec3(0.2f));
        floor_shader.set_vec3("dir_light.diffuse", glm::vec3(0.5f));
        floor_shader.set_vec3("dir_light.specular", glm::vec3(1.0f));

        // Point light properties.
        for (std::size_t i = 0; i < point_light_positions.size(); i++)
        {
            std::string attr_prefix{"point_lights[" + std::to_string(i) + "]."};

            floor_shader.set_vec3(attr_prefix + "position", point_light_positions[i]);
            floor_shader.set_vec3(attr_prefix + "ambient", glm::vec3(0.05f));
            floor_shader.set_vec3(attr_prefix + "diffuse", point_light_colors[i] * glm::vec3(0.5f));
            floor_shader.set_vec3(attr_prefix + "specular", point_light_colors[i] * glm::vec3(1.0f));
            floor_shader.set_float(attr_prefix + "constant", 1.0f);
            floor_shader.set_float(attr_prefix + "linear", 0.07f);
            floor_shader.set_float(attr_prefix + "quadratic", 0.017f);
        }

        // Spotlight properties.
        floor_shader.set_vec3("spotlight.position", camera_pos);  // TODO keep here
        floor_shader.set_vec3("spotlight.direction", camera_front);  // TODO keep here
        floor_shader.set_float("spotlight.inner_cutoff", glm::cos(glm::radians(12.5f)));
        floor_shader.set_float("spotlight.outer_cutoff", glm::cos(glm::radians(17.5f)));

        floor_shader.set_vec3("spotlight.ambient", glm::vec3(0.0f));
        floor_shader.set_vec3("spotlight.diffuse", glm::vec3(0.5f));
        floor_shader.set_vec3("spotlight.specular", glm::vec3(1.0f));

        floor_shader.set_float("spotlight.constant", 1.0f);
        floor_shader.set_float("spotlight.linear", 0.07f);
        floor_shader.set_float("spotlight.quadratic", 0.017f);

        // Material properties.
        floor_shader.set_float("material.shininess", 32.0f);

        floor_shader.set_mat4fv("projection", projection);
        floor_shader.set_mat4fv("view", view);
        floor_shader.set_mat4fv("model", model);

        room.draw();

        /*
         * Swap buffers and poll I/O events.
         */
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    /*
     * Clean up.
     */
    model_object.deinit();
    for (auto& point_light : point_lights)
        point_light.deinit();
    room.deinit();

    glfwTerminate();
    return 0;
}
