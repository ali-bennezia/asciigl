#! /bin/sh

echo Beginning compilation ...

cd bin

if ! [ -d static ]; then
	mkdir static
fi

if ! [ -d shared ]; then
	mkdir shared
fi

cd static

gcc -c ./../../src/asciigl.c ./../../src/utils.c \
	./../../src/render.c ./../../src/state.c \
	./../../src/images.c ./../../src/models.c -I"./../../include" 

ar rcs libasciigl-static.a asciigl.o utils.o render.o state.o images.o models.o

rm *.o

cd ../shared

gcc -fPIC -c ./../../src/asciigl.c ./../../src/utils.c ./../../src/render.c ./../../src/state.c ./../../src/images.c ./../../src/models.c -I"./../../include"
gcc -shared asciigl.o utils.o render.o state.o images.o models.o -o libasciigl-shared.so

rm *.o

cd ../..

echo Done building asciigl.
