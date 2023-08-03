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

gcc -c ./../../src/utils.c^
	./../../src/render.c ./../../src/state.c^
	./../../src/images.c ./../../src/models.c -I"./../../include" 

ar rcs libasciigl-static.a utils.o render.o state.o images.o models.o

DEL *.o

cd ../shared

gcc -fPIC -c ./../../src/utils.c ./../../src/render.c ./../../src/state.c ./../../src/images.c ./../../src/models.c -I"./../../include"
gcc -shared utils.o render.o state.o images.o models.o -o libasciigl-shared.so

DEL *.o

cd ../../

echo Done building asciigl.
