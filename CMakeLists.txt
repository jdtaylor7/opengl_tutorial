cmake_minimum_required(VERSION "3.14")

# Set project name.
project(OpenglTutorial)

# Specify C++ standard.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Build type.
set(CMAKE_BUILD_TYPE Release)

# Create compilation database for Sourcetrail.
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

# Add packages.
set(CMAKE_PREFIX_PATH /usr/lib/glfw)
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 CONFIG REQUIRED)
find_package(ASSIMP CONFIG REQUIRED)

# Add include directories.
include_directories(
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/third_party/GL"
    "${PROJECT_SOURCE_DIR}/third_party/glad/include"
    "${PROJECT_SOURCE_DIR}/third_party/glfw-3.3.2/include"
    "${PROJECT_SOURCE_DIR}/third_party/glm-0.9.9.8"
    "${PROJECT_SOURCE_DIR}/third_party/stb_image"
    "${PROJECT_SOURCE_DIR}/include"
)

# Add library directories.
link_directories(
    "${ASSIMP_LIBRARIES}"
)

# Add compiler/linker options.
set(CXXOPTS "-O2")
set(LINKOPTS "-Wl,-Bstatic -lm -lrt -Wl,-Bdynamic -ldl -lX11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXXOPTS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LINKOPTS}")
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")

# Add libraries.
add_library(glad OBJECT third_party/glad/src/glad.c)
add_library(stb_image OBJECT third_party/stb_image/stb_image.c)

# Link libraries for future targets. Just lazily linking them all here.
link_libraries(glad stb_image glfw assimp)

#
# Add executables.
#
# Getting started.
add_executable(window src/1_getting_started/1_hello_window/main.cpp)

add_executable(triangle src/1_getting_started/2_hello_triangle/1_triangle/main.cpp)
add_executable(square src/1_getting_started/2_hello_triangle/2_square/main.cpp)

add_executable(shaders1 src/1_getting_started/3_shaders/1_shader_updating/main.cpp)
add_executable(shaders2 src/1_getting_started/3_shaders/2_shader_interpolating/main.cpp)
add_executable(shaders3 src/1_getting_started/3_shaders/3_shader_from_file/main.cpp)

add_executable(textures1 src/1_getting_started/4_textures/1_one_texture/main.cpp)
add_executable(textures2 src/1_getting_started/4_textures/2_two_textures/main.cpp)

add_executable(transformations src/1_getting_started/5_transformations/main.cpp)

add_executable(coordinate_systems1 src/1_getting_started/6_coordinate_systems/1_single_plane/main.cpp)
add_executable(coordinate_systems2 src/1_getting_started/6_coordinate_systems/2_single_cube/main.cpp)
add_executable(coordinate_systems3 src/1_getting_started/6_coordinate_systems/3_many_cubes/main.cpp)

add_executable(camera1 src/1_getting_started/7_camera/1_automatic_camera/main.cpp)
add_executable(camera2 src/1_getting_started/7_camera/2_manual_camera/main.cpp)

# Lighting.
add_executable(colors src/2_lighting/1_colors/main.cpp)

add_executable(ambient src/2_lighting/2_basic_lighting/1_ambient/main.cpp)
add_executable(diffuse src/2_lighting/2_basic_lighting/2_diffuse/main.cpp)
add_executable(specular src/2_lighting/2_basic_lighting/3_specular/main.cpp)

add_executable(setting_materials src/2_lighting/3_materials/1_setting_materials/main.cpp)
add_executable(adjusting_light src/2_lighting/3_materials/2_adjusting_light/main.cpp)
add_executable(changing_color src/2_lighting/3_materials/3_changing_color/main.cpp)

add_executable(diffuse_map src/2_lighting/4_lighting_maps/1_diffuse/main.cpp)
add_executable(specular_map src/2_lighting/4_lighting_maps/2_specular/main.cpp)

add_executable(directional_light src/2_lighting/5_light_casters/1_directional/main.cpp)
add_executable(point_light src/2_lighting/5_light_casters/2_point/main.cpp)
add_executable(spotlight src/2_lighting/5_light_casters/3_spotlight/main.cpp)

add_executable(multiple_lights src/2_lighting/6_multiple_lights/main.cpp)

# Model loading.
add_executable(model src/3_model_loading/main.cpp)

# Advanced OpenGL.
add_executable(anti_aliasing src/4_advanced_opengl/11_anti_aliasing/main.cpp)

# Advanced lighting.
add_executable(shadow_mapping src/5_advanced_lighting/3_shadows/1_shadow_mapping/main.cpp)
