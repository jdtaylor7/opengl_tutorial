# Detect Linux.
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Linux)
	CFLAGS += -D LINUX
endif

# Detect architecture for Linux.
UNAME_M := $(shell uname -m)
ifeq ($(UNAME_M), x86_64)
	CFLAGS += -D AMD64
endif
ifneq ($(filter %86, $(UNAME_M)),)
	CFLAGS += -D IA32
endif

gl = third_party/GL
glad = third_party/glad
glfw = third_party/glfw-3.3.2
stb = third_party/stb_image
glm = third_party/glm-0.9.9.8

CC = clang
CXX = clang++
CFLAGS += -O2
CXXFLAGS = $(CFLAGS) -std=c++17
LINKOPTS = -L$(gl)/lib -L$(glfw)/build/lib -lGL -lglfw3 -Wl,-Bstatic -lm -lrt -Wl,-Bdynamic -ldl -lX11
includes = -I$(gl) -I$(glad)/include -I$(glfw)/include -I$(stb) -I$(glm) -Iinclude

all: window triangle triangle2 shaders1

glad.o: $(glad)/src/glad.c
	$(CC) -c $(CFLAGS) $(includes) $^

stb_image.o: $(stb)/stb_image.c
	$(CC) -c $(CFLAGS) $(includes) $^

window: glad.o src/hello_window/hello_window.cpp
	$(CXX) $(CXXFLAGS) $(includes)	$^ -o window.exe $(LINKOPTS)

triangle1: glad.o src/hello_triangle/1/hello_triangle1.cpp
	$(CXX) $(CXXFLAGS) $(includes)	$^ -o triangle1.exe $(LINKOPTS)

triangle2: glad.o src/hello_triangle/2/hello_triangle2.cpp
	$(CXX) $(CXXFLAGS) $(includes)	$^ -o triangle2.exe $(LINKOPTS)

shaders1: glad.o src/shaders/1/shaders1.cpp
	$(CXX) $(CXXFLAGS) $(includes)	$^ -o shaders1.exe $(LINKOPTS)

shaders2: glad.o src/shaders/2/shaders2.cpp
	$(CXX) $(CXXFLAGS) $(includes)	$^ -o shaders2.exe $(LINKOPTS)

shaders3: glad.o src/shaders/3/shaders3.cpp
	$(CXX) $(CXXFLAGS) $(includes)	$^ -o shaders3.exe $(LINKOPTS)

textures1: glad.o stb_image.o src/textures/1/textures1.cpp
	$(CXX) $(CXXFLAGS) $(includes)	$^ -o textures1.exe $(LINKOPTS)

textures2: glad.o stb_image.o src/textures/2/textures2.cpp
	$(CXX) $(CXXFLAGS) $(includes)	$^ -o textures2.exe $(LINKOPTS)

transformations: glad.o stb_image.o src/transformations/transformations.cpp
	$(CXX) $(CXXFLAGS) $(includes)	$^ -o transformations.exe $(LINKOPTS)

coordinate_systems1: glad.o stb_image.o src/coordinate_systems/1/coordinate_systems1.cpp
	$(CXX) $(CXXFLAGS) $(includes)	$^ -o coordinate_systems1.exe $(LINKOPTS)

coordinate_systems2: glad.o stb_image.o src/coordinate_systems/2/coordinate_systems2.cpp
	$(CXX) $(CXXFLAGS) $(includes)	$^ -o coordinate_systems2.exe $(LINKOPTS)

coordinate_systems3: glad.o stb_image.o src/coordinate_systems/3/coordinate_systems3.cpp
	$(CXX) $(CXXFLAGS) $(includes)	$^ -o coordinate_systems3.exe $(LINKOPTS)

camera1: glad.o stb_image.o src/camera/1/camera1.cpp
	$(CXX) $(CXXFLAGS) $(includes)	$^ -o camera1.exe $(LINKOPTS)

camera2: glad.o stb_image.o src/camera/2/camera2.cpp
	$(CXX) $(CXXFLAGS) $(includes)	$^ -o camera2.exe $(LINKOPTS)

clean:
	rm -vf *.o *.exe *.gch *.exe.stackdump
