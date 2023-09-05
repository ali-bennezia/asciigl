/* 

	This file is part of asciigl.

	asciigl is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

	asciigl is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with asciigl. If not, see <https://www.gnu.org/licenses/>.

*/


#include "./../include/config.h"
#include "./../include/state.h"
#include "./../include/utils.h"
#include "./../include/render.h"
#include "./../include/images.h"
#include "./../include/models.h"

//process

Model *mptr, *bptr;

void cleanup(){
    set_default_draw_color();
}

void render_frame(){
    if (get_depth_testing_state() == DEPTH_TESTING_STATE_ENABLED)
        clear_depth_buffer();

    clear_frame_buffer();
    set_default_draw_color();

  //  (&(mptr->rotation))->x += 4.0;
   // (&(mptr->rotation))->x += 4.0;
   // (&(mptr->rotation))->z += 4.0;
	translate_player(0.5, 0.4, 0);
    //set_player_rotation(-60, 0, 0);
	rotate_player(-1.2, 1, 0);

    draw();
    //char str[43] = "Properties\nX: 0, Y: 0, Z: 0\n> Hello World!\0";
    //draw_text( 10, 10, &str[0] );


    char* frame_string = NULL;
    size_t frame_string_size = 0;
    process_draw_string( &frame_string, &frame_string_size );
    printf( "%s", frame_string );
    free(frame_string);

    set_default_print_color();
}

void routine(){
    while (1){
	clear_console();
        render_frame();
        SLEEP(SLEEP_TIME);
    }
}

void init(){
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


    RGB white = {255, 255, 255};
    RGB red = {255, 0, 0};

    add_ambient_light("Light", 30, white);

    Vec3 normal = {5, 5, 1};
    add_directional_light("Some light", 100, vec3_normalize(normal), white);
    Vec3 pos = {1, 0, 2};
    //add_point_light("Some point light", 150, 10, pos, white);

    set_player_position(0,0,0);
    set_player_rotation(0,0,0);
    
    clear_console();

}

int main(int argc, char* argv[]){

    init();

    //debug

    Texture* tex = load_texture("test2.bmp");

    //mptr = gen_model();

    //mptr->rotation.y = 90;
    //mptr->position.z = 10.0;
    //mptr->position.x = 0.0;
    Vec3 unit_scale = { 1, 1, 1 };
    //mptr->scale = unit_scale;
    //mdl.texture = tex;
    

    bptr = gen_model();

    bptr->position.y = -4;
    bptr->position.z = 15.0;
    Vec3 bp_scale = { 10, 2, 10 };
    bptr->scale = bp_scale;

  //  mptr->texture = tex;

//    load_model_obj_strategy("cube.obj", mptr);
    load_model_obj_strategy("cube.obj", bptr);

    Model *blb = gen_model_billboard();

    blb->position.z = -10.0;
    blb->position.x = 5.0;
    blb->scale = unit_scale;
    blb->texture = tex;
    load_model_obj_strategy("cube.obj", blb);

    /*IntVec2 txt_pos = {
	10,
	10
    };

    RGB txt_col = {
	0, 
	255,
	0
    };

    gen_ui_text( "Properties\nX: 0, Y: 0, Z: 0\n> Hello World!\0", txt_pos, txt_col, 6 );

    IntVec2 frame_pos = {
	9,
	9
    };

    IntVec2 frame_size = {
	20,
	6
    };

    RGB frame_col = {
	0,
	255,
	0
    };

    gen_ui_frame( frame_pos, frame_size, frame_col, 5 );*/

    /*Texture *img_tex = load_texture( "test.bmp" );

    IntVec2 img_pos = {
	0,
	0
    };

    IntVec2 img_size = {
	120,
	40
    };

    RGB img_col = {
	255,
	255,
	255
    };

    gen_ui_image( img_pos, img_size, img_col, 10, img_tex );*/

    //Pre-begin

    set_player_rotation(-7, 0, 0);
    set_player_position(0, 0, -25);

    //Begin routine.
    routine();

    return 0;
}

