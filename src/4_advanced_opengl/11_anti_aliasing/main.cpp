#undef NDEBUG

#include <cassert>
#include <iostream>
#include <filesystem>
#include <memory>
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
#include "quad.hpp"
#include "room.hpp"
#include "shader.hpp"

constexpr float SCREEN_WIDTH = 800.0f;
constexpr float SCREEN_HEIGHT = 600.0f;

ModelSettings backpack("backpack", true, 0.5f);
ModelSettings drone("drone", false, 0.002f);

ModelSettings model_settings = drone;
bool show_mesh = false;

float room_scale_factor = 24.0f;

namespace fs = std::filesystem;
const fs::path shader_path = "src/4_advanced_opengl/11_anti_aliasing";
const fs::path texture_path = "assets/textures";

const fs::path plight_vshader_path = shader_path / "point_light.vs";
const fs::path plight_fshader_path = shader_path / "point_light.fs";
const fs::path main_vshader_path = shader_path / "main.vs";
const fs::path main_fshader_path = shader_path / "main.fs";
const fs::path shadow_vshader_path = shader_path / "shadow_depth.vs";
const fs::path shadow_fshader_path = shader_path / "shadow_depth.fs";
const fs::path quad_vshader_path = shader_path / "quad.vs";
const fs::path quad_fshader_path = shader_path / "quad.fs";

const fs::path model_directory = "assets/models/" + model_settings.name;
const fs::path model_obj_path = model_directory / (model_settings.name + ".obj");

const fs::path floor_texture_path = texture_path / "tile_floor";
const fs::path ceiling_texture_path = texture_path / "tile_floor";
const fs::path wall_texture_path = texture_path / "scifi_wall";

const fs::path floor_diffuse_path = floor_texture_path / "diffuse.png";
const fs::path floor_specular_path = floor_texture_path / "specular.png";
const fs::path ceiling_diffuse_path = ceiling_texture_path / "diffuse.png";
const fs::path ceiling_specular_path = ceiling_texture_path / "specular.png";
const fs::path wall_diffuse_path = wall_texture_path / "diffuse.png";
const fs::path wall_specular_path = wall_texture_path / "specular.png";

float yaw = -109.8f;
float pitch = -7.10f;

glm::vec3 camera_pos = glm::vec3(2.85f, 1.68f, 7.70f);
glm::vec3 camera_front = glm::vec3(-0.34f, -0.12f, -0.93f);
glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);

float delta_time = 0.0f;
float last_frame = 0.0f;

float camera_speed = 2.5f * delta_time;

float lastx = SCREEN_WIDTH / 2;
float lasty = SCREEN_HEIGHT / 2;

constexpr float mouse_sensitivity = 0.05f;

bool first_mouse = true;

float fov = 45.0f;

glm::vec3 model_pos(0.0f, 0.0f, 0.0f);

bool anti_aliasing_toggle = true;

/*
 * Light settings.
 */
// Shared.
const float light_attenuation_constant = 1.0f;
const float light_attenuation_linear = 0.07f;
const float light_attenuation_quadratic = 0.017f;

const glm::vec3 diffuse_light_intensity = glm::vec3(0.5f);
const glm::vec3 specular_light_intensity = glm::vec3(1.0f);

// Directional light.
const glm::vec3 directional_light_ambient_intensity = glm::vec3(0.2f);
const glm::vec3 directional_light_direction = glm::vec3(0.0f, -1.0f, 0.0f);

// Point lights.
const glm::vec3 point_light_ambient_intensity = glm::vec3(0.8f);
const std::vector<glm::vec3> point_light_colors{
    // glm::vec3(1.0f, 0.0f, 0.0f),
    // glm::vec3(0.0f, 1.0f, 0.0f),
    // glm::vec3(0.0f, 0.0f, 1.0f),
    // glm::vec3(1.0f, 1.0f, 0.0f),
    // glm::vec3(1.0f, 1.0f, 1.0f),
    // glm::vec3(1.0f, 1.0f, 1.0f),
    // glm::vec3(1.0f, 1.0f, 1.0f),
    // glm::vec3(1.0f, 1.0f, 1.0f),
    // glm::vec3(1.0f, 1.0f, 1.0f),
    // glm::vec3(1.0f, 0.875f, 0.0f),
    glm::vec3(0.529f, 0.808f, 0.922f),
};

const std::vector<glm::vec3> point_light_positions = {
    // glm::vec3( 9.0f, 9.0f,  9.0f),
    // glm::vec3( 9.0f, 9.0f, -9.0f),
    // glm::vec3(-9.0f, 9.0f,  9.0f),
    // glm::vec3(-9.0f, 9.0f, -9.0f),
    // glm::vec3( 9.0f, 9.0f,  0.0f),
    // glm::vec3( 0.0f, 9.0f,  9.0f),
    // glm::vec3(-9.0f, 9.0f,  0.0f),
    // glm::vec3( 0.0f, 9.0f, -9.0f),
    // glm::vec3( 0.0f, 9.0f,  0.0f),
    // glm::vec3( -4.0f, 2.0f, -2.0f),
    glm::vec3( 1.5f, 3.5f, 0.0f),
};

const float point_light_scale_factor = 0.2f;

// Spotlight.
const glm::vec3 spotlight_color = glm::vec3(1.0f);
const glm::vec3 spotlight_ambient_intensity = glm::vec3(0.0f);
glm::vec3 spotlight_position = camera_pos;
glm::vec3 spotlight_direction = camera_front;
const float spotlight_inner_cutoff = 12.5f;  // Degrees
const float spotlight_outer_cutoff = 17.5f;  // Degrees

/*
 * Shadow settings.
 */
// Depth map resolution.
const std::size_t shadow_width = 4096;
const std::size_t shadow_height = 4096;

// Light frustum settings.
float light_frustum_near_plane = 0.1f;
float light_frustum_far_plane = 30.0f;
float light_fov = 90.0f;
glm::mat4 light_projection = glm::perspective(
    glm::radians(light_fov),
    float(shadow_width / shadow_height),
    light_frustum_near_plane,
    light_frustum_far_plane);

/*
 * Declare objects.
 */
/*
 * Lights.
 */
// Directional lights.
std::unique_ptr<DirectionalLight> directional_light;

// Point lights.
std::vector<std::shared_ptr<PointLight>> point_lights;

// Spotlight.
std::unique_ptr<Spotlight> spotlight;

// Scene lighting.
std::unique_ptr<SceneLighting> scene_lighting;

// Quad. TODO for testing only.
std::unique_ptr<Quad> quad;

/*
 * Room.
 */
std::unique_ptr<Room> room;

/*
 * Model.
 */
std::unique_ptr<Model> model_object;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window)
{
    // Exit application.
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Camera speed modifier.
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera_speed = 5.0f * delta_time;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        camera_speed = 2.5f * delta_time;

    // Camera basic movement.
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera_pos += camera_speed * camera_front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera_pos -= camera_speed * camera_front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera_pos -= glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera_pos += glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;

    // Camera up/down.
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera_pos += camera_speed * camera_up;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera_pos -= camera_speed * camera_up;

    // Print camera info to terminal.
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
    {
        std::cout << "camera_pos: " << camera_pos.x << ", " << camera_pos.y << ", " << camera_pos.z << '\n';
        std::cout << "camera_front: " << camera_front.x << ", " << camera_front.y << ", " << camera_front.z << '\n';
        std::cout << "yaw: " << yaw << '\n';
        std::cout << "pitch: " << pitch << '\n';
        std::cout << '\n';
    }

    // Move drone up/down.
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        model_pos.y += camera_speed * 1.0f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        model_pos.y -= camera_speed * 1.0f;

    // Toggle anti-aliasing.
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        anti_aliasing_toggle = false;
    else if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE)
        anti_aliasing_toggle = true;
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

void render_scene(Shader* shader)
{
    if (!shader)
    {
        std::cerr << "main::render_scene: shader is NULL\n";
        return;
    }
    /*
     * Draw room.
     */
    shader->use();

    // Position properties.
    shader->set_vec3("view_pos", camera_pos);

    // Material properties.
    shader->set_float("material.shininess", 32.0f);

    // Render room.
    if (!room)
    {
        std::cerr << "main::render_scene: room is NULL\n";
        return;
    }
    room->draw(shader);

    /*
     * Draw model.
     */
    shader->use();

    // Position properties.
    shader->set_vec3("view_pos", camera_pos);

    // Set model matrix.
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, model_pos);
    model = glm::scale(model, glm::vec3(model_settings.scale_factor));
    shader->set_mat4fv("model", model);

    // Render model.
    if (!model_object)
    {
        std::cerr << "main::render_scene: model_object is NULL\n";
        return;
    }
    model_object->draw(shader);
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
    glfwWindowHint(GLFW_SAMPLES, 4);

    /*
     * GLFW window creation.
     */
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello Triangle", NULL, NULL);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window\n";
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
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    /*
     * Set global OpenGL state.
     */
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    if (show_mesh)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    /*
     * Set up shadow mapping.
     */
    // Create framebuffer for depth map.
    unsigned int depth_map_fbo;
    glGenFramebuffers(1, &depth_map_fbo);

    // Create texture for depth map.
    unsigned int depth_map;
    glGenTextures(1, &depth_map);
    glBindTexture(GL_TEXTURE_2D, depth_map);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_width,
        shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    std::vector<float> border_color = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color.data());
    glBindTexture(GL_TEXTURE_2D, 0);

    // Attach depth texture as framebuffer's depth buffer.
    glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "main: Framebuffer incomplete\n";
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /*
     * Create shader programs.
     */
    auto plight_shader = std::make_unique<Shader>(plight_vshader_path.string(), plight_fshader_path.string());
    auto main_shader = std::make_unique<Shader>(main_vshader_path.string(), main_fshader_path.string());
    auto shadow_shader = std::make_unique<Shader>(shadow_vshader_path.string(), shadow_fshader_path.string());
    auto quad_shader = std::make_unique<Shader>(quad_vshader_path.string(), quad_fshader_path.string());

    /*
     * Initialize lights.
     */
    // Directional lights.
    directional_light = std::make_unique<DirectionalLight>(
        directional_light_direction,
        directional_light_ambient_intensity,
        diffuse_light_intensity,
        specular_light_intensity);

    // Point lights.
    assert(point_light_positions.size() == point_light_colors.size());
    for (std::size_t i = 0; i < point_light_positions.size(); i++)
    {
        auto point_light = std::make_shared<PointLight>(
            point_light_positions[i],
            point_light_colors[i],
            point_light_scale_factor,
            point_light_ambient_intensity,
            diffuse_light_intensity,
            specular_light_intensity,
            light_attenuation_constant,
            light_attenuation_linear,
            light_attenuation_quadratic);
        point_light->init();
        point_lights.push_back(point_light);
    }

    // Spotlight.
    spotlight = std::make_unique<Spotlight>(
        spotlight_position,
        spotlight_direction,
        spotlight_inner_cutoff,
        spotlight_outer_cutoff,
        spotlight_ambient_intensity,
        diffuse_light_intensity,
        specular_light_intensity,
        light_attenuation_constant,
        light_attenuation_linear,
        light_attenuation_quadratic);

    // Scene lighting.
    scene_lighting = std::make_unique<SceneLighting>(
        directional_light.get(),
        point_lights,
        spotlight.get());

    /*
     * Initialize room.
     */
    room = std::make_unique<Room>(floor_diffuse_path,
        floor_specular_path,
        ceiling_diffuse_path,
        ceiling_specular_path,
        wall_diffuse_path,
        wall_specular_path,
        scene_lighting.get(),
        room_scale_factor);
    room->init();

    /*
     * Initialize model.
     */
    model_object = std::make_unique<Model>(model_obj_path,
        model_settings.flip_textures,
        scene_lighting.get());
    model_object->init();

    /*
     * Initialize quad. TODO for testing only.
     */
    quad = std::make_unique<Quad>();
    quad->init();

    /*
     * Render loop.
     */
    while (!glfwWindowShouldClose(window))
    {
        float current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        // Toggle anti-aliasing.
        if (anti_aliasing_toggle)
        {
            // Enable AA.
            glEnable(GL_MULTISAMPLE);
            main_shader->use();
            main_shader->set_bool("smooth_shadows", true);
        }
        else
        {
            // Disable AA.
            glDisable(GL_MULTISAMPLE);
            main_shader->use();
            main_shader->set_bool("smooth_shadows", false);
        }

        /*
         * Input.
         */
        process_input(window);

        // Update spotlight based on camera movement.
        spotlight->update(camera_pos, camera_front);

        /*
         * Generate depth buffer for shadows.
         */
        // Set up light perspective matrix. This part is a bit of a hack since
        // we're pretending a point light is a directional light (by using a
        // lookAt matrix which always looks at the model). This is fine since we
        // only have one model in the scene. It also means we can use shadow
        // mapping instead of point shadows, which is simpler.
        glm::mat4 light_view = glm::lookAt(
            point_light_positions[0], model_pos, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 light_space_matrix = light_projection * light_view;

        // Pass uniforms to shader.
        shadow_shader->use();
        shadow_shader->set_mat4fv("light_space_matrix", light_space_matrix);

        glViewport(0, 0, shadow_width, shadow_height);
        glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
        glClear(GL_DEPTH_BUFFER_BIT);

        // Render scene to shadow map. Cull front faces during to eliminate
        // potential peter panning.
        glCullFace(GL_FRONT);
        render_scene(shadow_shader.get());
        glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        /*
         * Render.
         */
        main_shader->use();

        // Reset viewport.
        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        // Color buffer.
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Initial projection and view matrix definitions.
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        // Set view and projection matrices. Model matrix set per object in
        // render_scene function.
        view = glm::lookAt(camera_pos, camera_pos + camera_front, camera_up);
        projection = glm::perspective(glm::radians(fov), SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);

        // Assign projection and view matrices.
        main_shader->set_mat4fv("projection", projection);
        main_shader->set_mat4fv("view", view);

        // Pass light space matrix to main shader.
        main_shader->set_mat4fv("light_space_matrix", light_space_matrix);

        // Pass depth map to objects, to render shadows.
        room->set_depth_map(depth_map);
        model_object->set_depth_map(depth_map);

        // Render scene normally.
        render_scene(main_shader.get());

        /*
         * Draw point lights.
         */
        plight_shader->use();

        // Set MVP matrices.
        plight_shader->set_mat4fv("projection", projection);
        plight_shader->set_mat4fv("view", view);

        // Render point light.
        for (auto& point_light : point_lights)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, point_light->position);
            model = glm::scale(model, glm::vec3(point_light->scale_factor));
            plight_shader->set_mat4fv("model", model);
            plight_shader->set_vec3("color", point_light->color);
            point_light->draw();
        }

        // // Render quad. TODO for testing only.
        // quad_shader->use();
        // quad_shader->set_float("near_plane", light_frustum_near_plane);
        // quad_shader->set_float("far_plane", light_frustum_far_plane);
        // quad->set_depth_map(depth_map);
        // quad->draw(quad_shader.get());

        /*
         * Swap buffers and poll I/O events.
         */
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    /*
     * Clean up.
     */
    model_object->deinit();
    for (auto& point_light : point_lights)
        point_light->deinit();
    room->deinit();

    glfwTerminate();
    return 0;
}
