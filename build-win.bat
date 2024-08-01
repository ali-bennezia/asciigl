echo off
echo Beginning compilation ...

cd bin

if not exist static (
	mkdir static
)

if not exist shared (
	mkdir shared
)

cd static

gcc -c ./../../src/asciigl.c ./../../src/utils.c^
	./../../src/render.c ./../../src/state.c^
	./../../src/images.c ./../../src/meshes.c -I"./../../include" 

ar rcs libasciigl-static.a asciigl.o utils.o render.o state.o images.o meshes.o

DEL *.o

cd ../shared

gcc -fPIC -c ./../../src/asciigl.c ./../../src/utils.c ./../../src/render.c ./../../src/state.c ./../../src/images.c ./../../src/meshes.c -I"./../../include"
gcc -shared asciigl.o utils.o render.o state.o images.o meshes.o -o libasciigl-shared.so

DEL *.o

cd ../../

echo Done building asciigl.
