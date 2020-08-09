Learning how to use OpenGL. Following the LearnOpenGL tutorial
[here](https://learnopengl.com/).

## Dependencies

### Linux

**TODO: add**

### Cygwin

* libGL-devel
* libGL1
* libiconv-devel (?)
* libX11-devel
* libXrandr-devel
* libXinerama-devel
* libXcursor-devel
* libXi-devel
* xorg-server
* xinit

##### Installing GLFW with CMake

* Download GLFW source code: `git clone https://github.com/glfw/glfw`
* Create build directory: `mkdir <install_dir>` (I used `/usr/lib/glfw`)
* In root directory: `cmake -B build -DCMAKE_INSTALL_PREFIX=<install_dir>
-DGLFW_BUILD_DOCS=OFF -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF
-DBUILD_SHARED_LIBS=OFF`
* `cd build`
* `make glfw`
* `make install`

##### Installing Assimp with CMake

**TODO: add**

## Building

### Platform-agnostic

Run the build script:

`./build.sh`

## Running

### Linux

`./build/<target>`

### Cygwin

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
