/* 

	This file is part of asciigl.

	asciigl is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

	asciigl is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with asciigl. If not, see <https://www.gnu.org/licenses/>.

*/

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

#define FRAME_HEIGHT 40
#define FRAME_WIDTH 120

#define TOTAL_FRAGMENTS_PER_FRAME FRAME_HEIGHT*FRAME_WIDTH

#define FRAME_HEIGHT_WIDTH_RATIO (float)FRAME_HEIGHT/(float)FRAME_WIDTH
#define FRAG_HEIGHT_WIDTH_RATIO 1.66666666667f

#endif
