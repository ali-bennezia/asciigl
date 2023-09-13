/* 

	This file is part of asciigl.

	asciigl is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

	asciigl is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with asciigl. If not, see <https://www.gnu.org/licenses/>.

*/

#include "./../include/asciigl.h"

//process

static void cleanup(){
    set_default_draw_color();
}

static void render_frame(){
    if (get_depth_testing_state() == DEPTH_TESTING_STATE_ENABLED)
        clear_depth_buffer();

    clear_frame_buffer();
    set_default_draw_color();

    draw();

    char* frame_string = NULL;
    size_t frame_string_size = 0;
    process_draw_string( &frame_string, &frame_string_size );
    printf( "%s", frame_string );
    free(frame_string);

    set_default_print_color();
}

void asciigl_process_frame(){
	clear_console();
	render_frame();
	SLEEP(SLEEP_TIME);
}

void asciigl_init(){
    //init

    atexit( cleanup );

    init_workspace();
    init_light_arrays();

    clear_frame_buffer();
    clear_depth_buffer();
    clear_ui_layers_buffer();

    set_depth_testing_state( DEPTH_TESTING_STATE_ENABLED );

    set_frustum_near_plane( 0.01 );
    set_frustum_far_plane( 1000 );
    set_frustum_FOV( 60.0 );

    set_player_position(0,0,0);
    set_player_rotation(0,0,0);
    
    clear_console();
}
