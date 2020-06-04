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
    "${PROJECT_SOURCE_DIR}/third_party/glm-0.9.9.8/glm"
    "${PROJECT_SOURCE_DIR}/third_party/stb_image"
    "${PROJECT_SOURCE_DIR}/include")

# Add directories in which linker should look for libraries.
# link_directories(
#     ${PROJECT_SOURCE_DIR}/third_party/glfw-3.3.2/build/lib  # -lglfw3
#     ${PROJECT_SOURCE_DIR}/third_party/GL/lib                # -lGL
# )

# Add compiler/linker options.
set(CXXOPTS "-O2")
# set(LINKOPTS "-L${PROJECT_SOURCE_DIR}/third_party/GL/lib -L${PROJECT_SOURCE_DIR}/third_party/glfw-3.3.2/build/lib -lGL -lglfw3 -Wl,-Bstatic -lm -lrt -Wl,-Bdynamic -ldl -lX11")
# set(LINKOPTS "-L${PROJECT_SOURCE_DIR}/third_party/glfw-3.3.2/build/lib -lglfw3 -Wl,-Bstatic -lm -lrt -Wl,-Bdynamic -ldl -lX11")
set(LINKOPTS "-Wl,-Bstatic -lm -lrt -Wl,-Bdynamic -ldl -lX11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXXOPTS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LINKOPTS}")
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")

# Add libraries.
add_library(glad OBJECT third_party/glad/src/glad.c)
# add_library(stb_image OBJECT third_party/stb_image/stb_image.c)

# Add executables.
add_executable(window src/hello_window/hello_window.cpp)

# Find libraries
# find_library(GL_lib GL ${PROJECT_SOURCE_DIR}/third_party/GL/lib)
# find_library(glfw3_lib glfw3 ${PROJECT_SOURCE_DIR}/third_party/glfw-3.3.2/build/lib)

# Link libraries.
target_link_libraries(window $<TARGET_OBJECTS:glad> glfw)
