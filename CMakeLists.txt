cmake_minimum_required(VERSION "3.14")

# Set project name.
project(OpenglTutorial)

# Specify C++ standard.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Create compilation database for Sourcetrail.
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

# Add packages.
find_package(OpenGL REQUIRED)

set(CMAKE_PREFIX_PATH /usr/lib/glfw)
find_package(glfw3 3.3 REQUIRED)

# Add include directories.
include_directories(
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/third_party/assimp-3.1.1/include"
    "${PROJECT_SOURCE_DIR}/third_party/GL"
    "${PROJECT_SOURCE_DIR}/third_party/glad/include"
    "${PROJECT_SOURCE_DIR}/third_party/glfw-3.3.2/include"
    "${PROJECT_SOURCE_DIR}/third_party/glm-0.9.9.8"
    "${PROJECT_SOURCE_DIR}/third_party/stb_image"
    "${PROJECT_SOURCE_DIR}/include"
)

# Add library directories.
link_directories(
    "${PROJECT_SOURCE_DIR}/third_party/assimp-3.1.1/lib"
)

# Add compiler/linker options.
set(CXXOPTS "-O2")
set(LINKOPTS "-Wl,-Bstatic -lm -lrt -Wl,-Bdynamic -ldl -lX11 -lassimp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXXOPTS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LINKOPTS}")
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")

# Add libraries.
add_library(glad OBJECT third_party/glad/src/glad.c)
add_library(stb_image OBJECT third_party/stb_image/stb_image.c)

# Link libraries for future targets. Just lazily linking them all here.
link_libraries(glad stb_image glfw)

#
# Add executables.
#
# Getting started.
add_executable(window src/getting_started/hello_window/hello_window.cpp)

add_executable(triangle1 src/getting_started/hello_triangle/1/hello_triangle1.cpp)
add_executable(triangle2 src/getting_started/hello_triangle/2/hello_triangle2.cpp)

add_executable(shaders1 src/getting_started/shaders/1/shaders1.cpp)
add_executable(shaders2 src/getting_started/shaders/2/shaders2.cpp)
add_executable(shaders3 src/getting_started/shaders/3/shaders3.cpp)

add_executable(textures1 src/getting_started/textures/1/textures1.cpp)
add_executable(textures2 src/getting_started/textures/2/textures2.cpp)

add_executable(transformations src/getting_started/transformations/transformations.cpp)

add_executable(coordinate_systems1 src/getting_started/coordinate_systems/1/coordinate_systems1.cpp)
add_executable(coordinate_systems2 src/getting_started/coordinate_systems/2/coordinate_systems2.cpp)
add_executable(coordinate_systems3 src/getting_started/coordinate_systems/3/coordinate_systems3.cpp)

add_executable(camera1 src/getting_started/camera/1/camera1.cpp)
add_executable(camera2 src/getting_started/camera/2/camera2.cpp)

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
