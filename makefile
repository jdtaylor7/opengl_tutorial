gl_inc = third_party/GL
gl_lib = third_party/GL/lib
glad_inc = third_party/glad/include
glad_src = third_party/glad/src
glfw_inc = third_party/glfw-3.3.2/include
glfw_lib = third_party/glfw-3.3.2/build/lib

CFLAGS = -O2
CXXFLAGS = -std=c++17 -O2
LINKOPTS = -L$(gl_lib) -L$(glfw_lib) -lGL -lglfw3 -Wl,-Bstatic -lm -lrt -Wl,-Bdynamic -ldl -lX11
includes = -I$(gl_inc) -I$(glad_inc) -I$(glfw_inc) -Iinclude

all: window triangle triangle2 shaders1

glad.o: $(glad_src)/glad.c
	clang -c $(CFLAGS) -I$(glad_inc) $^

stb_image.o: src/stb_image.c
	clang -c $(CFLAGS) -Iinclude $^

window: glad.o src/hello_window.cpp
	clang++ $(CXXFLAGS) $(includes)	$^ -o window.exe $(LINKOPTS)

triangle1: glad.o src/hello_triangle1.cpp
	clang++ $(CXXFLAGS) $(includes)	$^ -o triangle1.exe $(LINKOPTS)

triangle2: glad.o src/hello_triangle2.cpp
	clang++ $(CXXFLAGS) $(includes)	$^ -o triangle2.exe $(LINKOPTS)

shaders1: glad.o src/shaders1.cpp
	clang++ $(CXXFLAGS) $(includes)	$^ -o shaders1.exe $(LINKOPTS)

shaders2: glad.o src/shaders2.cpp
	clang++ $(CXXFLAGS) $(includes)	$^ -o shaders2.exe $(LINKOPTS)

shaders3: glad.o src/shaders3/shaders3.cpp
	clang++ $(CXXFLAGS) $(includes)	$^ -o shaders3.exe $(LINKOPTS)

textures1: glad.o stb_image.o src/textures1/textures1.cpp
	clang++ $(CXXFLAGS) $(includes)	$^ -o textures1.exe $(LINKOPTS)

clean:
	rm -vf *.o *.exe *.gch *.exe.stackdump
