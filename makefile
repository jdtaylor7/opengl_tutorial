gl = third_party/GL
glad = third_party/glad
glfw = third_party/glfw-3.3.2
stb = third_party/stb_image
glm = third_party/glm-0.9.9.8

CFLAGS = -O2
CXXFLAGS = -std=c++17 -O2
LINKOPTS = -L$(gl)/lib -L$(glfw)/build/lib -lGL -lglfw3 -Wl,-Bstatic -lm -lrt -Wl,-Bdynamic -ldl -lX11
includes = -I$(gl) -I$(glad)/include -I$(glfw)/include -I$(stb) -I$(glm) -Iinclude

all: window triangle triangle2 shaders1

glad.o: $(glad)/src/glad.c
	clang -c $(CFLAGS) $(includes) $^

stb_image.o: $(stb)/stb_image.c
	clang -c $(CFLAGS) $(includes) $^

window: glad.o src/hello_window/hello_window.cpp
	clang++ $(CXXFLAGS) $(includes)	$^ -o window.exe $(LINKOPTS)

triangle1: glad.o src/hello_triangle/1/hello_triangle1.cpp
	clang++ $(CXXFLAGS) $(includes)	$^ -o triangle1.exe $(LINKOPTS)

triangle2: glad.o src/hello_triangle/2/hello_triangle2.cpp
	clang++ $(CXXFLAGS) $(includes)	$^ -o triangle2.exe $(LINKOPTS)

shaders1: glad.o src/shaders/1/shaders1.cpp
	clang++ $(CXXFLAGS) $(includes)	$^ -o shaders1.exe $(LINKOPTS)

shaders2: glad.o src/shaders/2/shaders2.cpp
	clang++ $(CXXFLAGS) $(includes)	$^ -o shaders2.exe $(LINKOPTS)

shaders3: glad.o src/shaders/3/shaders3.cpp
	clang++ $(CXXFLAGS) $(includes)	$^ -o shaders3.exe $(LINKOPTS)

textures1: glad.o stb_image.o src/textures/1/textures1.cpp
	clang++ $(CXXFLAGS) $(includes)	$^ -o textures1.exe $(LINKOPTS)

textures2: glad.o stb_image.o src/textures/2/textures2.cpp
	clang++ $(CXXFLAGS) $(includes)	$^ -o textures2.exe $(LINKOPTS)

transformations: glad.o stb_image.o src/transformations/transformations.cpp
	clang++ $(CXXFLAGS) $(includes)	$^ -o transformations.exe $(LINKOPTS)

coordinate_systems1: glad.o stb_image.o src/coordinate_systems/1/coordinate_systems.cpp
	clang++ $(CXXFLAGS) $(includes)	$^ -o coordinate_systems1.exe $(LINKOPTS)

coordinate_systems2: glad.o stb_image.o src/coordinate_systems/2/coordinate_systems.cpp
	clang++ $(CXXFLAGS) $(includes)	$^ -o coordinate_systems2.exe $(LINKOPTS)

clean:
	rm -vf *.o *.exe *.gch *.exe.stackdump
