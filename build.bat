cd bin/shared
gcc -c  ./../../src/main.c ./../../src/utils.c ./../../src/utils.h ./../../src/config.h ./../../src/render.c ./../../src/render.h ./../../src/state.c ./../../src/state.h  -fPIC
ar rcs ../asciigl.a main.o utils.o config.o render.o state.o
cd ../../