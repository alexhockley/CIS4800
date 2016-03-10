
all: a

TARGET = -mmacosx-version-min=10.8
LIBS = -F/System/Library/Frameworks -framework OpenGL -framework GLUT

a: a.c
	clang a.c -o a2 $(LIBS) $(TARGET)

scene: scene.c
	clang scene.c -o scene $(LIBS) $(TARGET)

