gl_inc = third_party/GL
gl_lib = third_party/GL/lib
glad_inc = third_party/glad/include
glad_src = third_party/glad/src
glfw_inc = third_party/glfw-3.3.2/include
glfw_lib = third_party/glfw-3.3.2/build/lib
CFLAGS = -O2
CXXFLAGS = -std=c++17 -O2
LINKOPTS = -L$(gl_lib) -L$(glfw_lib) -lGL -lglfw3 -Wl,-Bstatic -lm -lrt -Wl,-Bdynamic -ldl -lX11
includes = -I$(gl_inc) -I$(glad_inc) -I$(glfw_inc)

all: window

glad.o: $(glad_src)/glad.c
	clang -c $(CFLAGS) -I$(glad_inc) $^

window: glad.o src/hello_window.cpp
	clang++ $(CXXFLAGS) $(includes)	$^ -o hello_window.exe $(LINKOPTS)

triangle: glad.o src/hello_triangle.cpp
	clang++ $(CXXFLAGS) $(includes)	$^ -o hello_triangle.exe $(LINKOPTS)

clean:
	rm -vf *.o *.exe *.gch *.exe.stackdump
