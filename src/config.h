#ifndef CONFIG_HEADERGUARD
#define CONFIG_HEADERGUARD

//config


//macros config

#define FPS 80

//macros & cross-platform include handling

#ifdef _WIN32

#include <windows.h>
#define CLEAR_CMD "cls"
#define SLEEP(INT) Sleep(INT)
#define SLEEP_TIME 1000.0/FPS

#elif defined linux

#include <unistd.h>
#define CLEAR_CMD "clear"
#define SLEEP_TIME 1.0/FPS
#define SLEEP(INT) sleep(INT)

#endif


//frame buffer

#define FRAME_HEIGHT 20
#define FRAME_WIDTH 60

#define TOTAL_FRAGMENTS_PER_FRAME FRAME_HEIGHT*FRAME_WIDTH

#define FRAME_HEIGHT_WIDTH_RATIO (float)FRAME_HEIGHT/(float)FRAME_WIDTH
#define FRAG_HEIGHT_WIDTH_RATIO 1.66666666667f

#endif
