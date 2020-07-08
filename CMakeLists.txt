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
    "${PROJECT_SOURCE_DIR}/third_party/GL"
    "${PROJECT_SOURCE_DIR}/third_party/glad/include"
    "${PROJECT_SOURCE_DIR}/third_party/glfw-3.3.2/include"
    "${PROJECT_SOURCE_DIR}/third_party/glm-0.9.9.8"
    "${PROJECT_SOURCE_DIR}/third_party/stb_image"
    "${PROJECT_SOURCE_DIR}/include")

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

# Add executables.
add_executable(window src/hello_window/hello_window.cpp)

add_executable(triangle1 src/hello_triangle/1/hello_triangle1.cpp)
add_executable(triangle2 src/hello_triangle/2/hello_triangle2.cpp)

add_executable(shaders1 src/shaders/1/shaders1.cpp)
add_executable(shaders2 src/shaders/2/shaders2.cpp)
add_executable(shaders3 src/shaders/3/shaders3.cpp)

add_executable(textures1 src/textures/1/textures1.cpp)
add_executable(textures2 src/textures/2/textures2.cpp)

add_executable(transformations src/transformations/transformations.cpp)

add_executable(coordinate_systems1 src/coordinate_systems/1/coordinate_systems1.cpp)
add_executable(coordinate_systems2 src/coordinate_systems/2/coordinate_systems2.cpp)
add_executable(coordinate_systems3 src/coordinate_systems/3/coordinate_systems3.cpp)

add_executable(camera1 src/camera/1/camera1.cpp)
add_executable(camera2 src/camera/2/camera2.cpp)

# Link libraries.
target_link_libraries(window glad glfw)

target_link_libraries(triangle1 glad glfw)
target_link_libraries(triangle2 glad glfw)

target_link_libraries(shaders1 glad glfw)
target_link_libraries(shaders2 glad glfw)
target_link_libraries(shaders3 glad glfw)

target_link_libraries(textures1 glad stb_image glfw)
target_link_libraries(textures2 glad stb_image glfw)

target_link_libraries(transformations glad stb_image glfw)

target_link_libraries(coordinate_systems1 glad stb_image glfw)
target_link_libraries(coordinate_systems2 glad stb_image glfw)
target_link_libraries(coordinate_systems3 glad stb_image glfw)

target_link_libraries(camera1 glad stb_image glfw)
target_link_libraries(camera2 glad stb_image glfw)
