GCC_OPTIONS=-Wall -pedantic -I include
GL_OPTIONS=-lGLEW -lGL -lglut
OPTIONS=$(GCC_OPTIONS) $(GL_OPTIONS)

default: run-p1 run-p2

assignment9p1:
	g++ assignment9p1.cpp BezierPatch.cpp BezierPatchParser.cpp Common/InitShader.o $(OPTIONS) -o assignment9p1

assignment9p2:
	g++ assignment9p2.cpp Model.cpp SMFParser.cpp Common/InitShader.o $(OPTIONS) -o assignment9p2

run-p1: assignment9p1
	./assignment9p1

run-p2: assignment9p2
	./assignment9p2

shader:
	Common/./Compile

clean:
	rm -rf assignment9p1 assignment9p2
