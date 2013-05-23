GCC_OPTIONS=-Wall -pedantic -I include
GL_OPTIONS=-lGLEW -lGL -lglut
OPTIONS=$(GCC_OPTIONS) $(GL_OPTIONS)

default: run

assignment7:
	g++ assignment7.cpp BezierPatch.cpp BezierPatchParser.cpp Common/InitShader.o $(OPTIONS) -o assignment7

run: assignment7
	./assignment7

shader:
	Common/./Compile

clean:
	rm -rf assignment7
