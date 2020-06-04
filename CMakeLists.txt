cmake_minimum_required(VERSION "3.14")

# Set project name.
project(OpenglTutorial)

# Specify C++ standard.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

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
add_executable(camera2 src/camera/2/camera2.cpp)

# Link libraries.
target_link_libraries(window glad glfw)
target_link_libraries(triangle1 glad glfw)
target_link_libraries(camera2 glad stb_image glfw)
