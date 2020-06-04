Learning how to use OpenGL! Following the LearnOpenGL tutorial
[here](https://learnopengl.com/).

### Cygwin packages to build GLFW
* libX11-devel
* libXrandr-devel
* libXinerama-devel
* libXcursor-devel
* libXi-devel

### Configuring GLFW
* BUILD_SHARED_LIBS: ON
* CMAKE_BUILD_TYPE: Release
* CMAKE_INSTALL_PREFIX: /usr/local
* GLFW_BUILD_DOCS: OFF
* GLFW_BUILD_EXAMPLES: OFF
* GLFW_BUILD_TESTS: OFF
* GLFW_INSTALL: ON
* GLFW_USE_OSMESA: OFF
* GLFW_USE_WAYLAND: OFF
* GLFW_VULKAN_STATIC: OFF

* Checked SHARED_LIBS: ON + BUILD_TYPE: Release
* Checked SHARED_LIBS: OFF + BUILD_TYPE: Release
* Checked SHARED_LIBS: ON + BUILD_TYPE: None
* Checked SHARED_LIBS: OFF + BUILD_TYPE: None

### Cygwin packages to run GLFW
* xorg-server
* xinit

### Starting xinit server
* Set following line in `~/.bashrc`:
    * `export DISPLAY=:0.0`
* Run `xinit -- -multiwindow &` to start the X server in the background
    * Alternative: Run `startx <executable> -- -multiwindow` to run client
    directly
* To run fullscreen, use `-fullscreen` as a server command with either `xinit`
or `startx`
* Can also use `startxwin <executable>` to run single client program in
multiwindow mode
* [Documentation](http://x.cygwin.com/docs/ug/using.html#using-starting) on
Cygwin site

### Nvidia drivers? --> don't need them yet at least
* libOpenCL-devel?
* libOpenCL1?

### OpenGL packages
* libGLU-devel (nope)
* libGLU1 (nope)
* libglut-devel (nope)
* libglut3 (nope)
* libGLw-devel (nope)
* libGLw1 (nope)
* libGL-devel (already installed)
* libGL1 (already installed)
* libiconv-devel (something links in the iconv dll, not sure what though)

### Setting up OpenGL in Cygwin
* Main resource: [Cygwin/X User's Guide](http://x.cygwin.com/docs/ug/using-glx.html)
* In Cygwin, OpenGL has to be run in windowed mode in order to use hardware
acceleration rendering
* Can check rendering method by running `/cygdrive/c/glfw-3.3.2/build/tests/glfwinfo.exe` while an X server is running:
    * Hardware (GPU)
        * OpenGL context version string: "4.6.0 NVIDIA 423.00"
        * OpenGL context version parsed by GLFW: 4.6.0
        * OpenGL context renderer string: "GeForce GTX 1070/PCIe/SSE2"
        * OpenGL context vender string: "NVIDIA Corporation"
        * OpenGL context shading language version: "4.60 NVIDIA"
    * Software (Mesa)
        * OpenGL context version string: "3.1 Mesa 19.1.6"
        * OpenGL context version parsed by GLFW: 3.1.0
        * OpenGL context renderer string: "llvmpipe (LLVM 8.0, 256 bits)"
        * OpenGL context vender string: "VMware, Inc."
        * OpenGL context shading language version: "1.40"
* Want to use direct, hardware accelerated rendering
    * Start server with `-wgl` option (default)

### Open issues
* "libGL error: Screen is not Windows-DRI capable" (doesn't seem to matter)
* Some GLFW examples don't work properly
    * `opacity.exe`
    * maybe others

### Configuring OpenGL with CMake

##### Option 1 (preferred): Use CMake package
`find_package(OpenGL REQUIRED)`
`target_link_libraries(<executable> OpenGL::GL)`

##### Option 2: Find and install library
`set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}
-L${PROJECT_SOURCE_DIR}/third_party/GL/lib -lGL")`
`find_library(GL_lib GL ${PROJECT_SOURCE_DIR}/third_party/GL/lib)`
`target_link_libraries(<executable> ${GL_lib})`

### Installing GLFW to work with CMake

##### Option 1 (preferred): Use CMake package
Build and install glfw with CMake.
* Download glfw source code.
* Create build directory: `mkdir <install dir>` (for my Cygwin system I used
`/usr/lib/glfw`)
* In root directory: `cmake
-B build
-DCMAKE_INSTALL_PREFIX=<install dir>
-DGLFW_BUILD_DOCS=OFF
-DGLFW_BUILD_EXAMPLES=OFF
-DGLFW_BUILD_TESTS=OFF
-DBUILD_SHARED_LIBS=OFF`
* `cd build`
* `make glfw`
* `make install`

Find the glfw package. Must be version 3.2 or later since that version was the
first to include the option to create a CMake package.
`find_package(glfw3 3.3 REQUIRED)`
`target_link_libraries(<executable> glfw)`

##### Option 2: Find and install library
`set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}
-L${PROJECT_SOURCE_DIR}/third_party/glfw-<version>/build/lib -lGL")`
`find_library(glfw3_lib glfw3
${PROJECT_SOURCE_DIR}/third_party/glfw-<version>/build/lib)`
`target_link_libraries(<executable> ${glfw3_lib})`
