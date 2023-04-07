/* 

	This file is part of asciigl.

	asciigl is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

	asciigl is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with asciigl. If not, see <https://www.gnu.org/licenses/>.

*/


#include "config.h"
#include "state.h"
#include "utils.h"
#include "render.h"
#include "images.h"
#include "models.h"

//process

Model* mptr;

void cleanup(){
    set_default_draw_color();
}

void render_frame(){
    if (get_depth_testing_state() == DEPTH_TESTING_STATE_ENABLED)
        clear_depth_buffer();

    clear_frame_buffer();
    //get_frame_buffer()[TOTAL_FRAGMENTS_PER_FRAME] = '\0';

    (&(mptr->rotation))->y += 4.0;
    (&(mptr->rotation))->x += 4.0;
    (&(mptr->rotation))->z += 4.0;

    //translate_player(0.05, 0, 0.05);
    //rotate_player(0, 10, 0);

    draw_model(*mptr);

    //printf("%s", get_frame_buffer());
    char* frame_string = NULL;
    size_t frame_string_size = 0;
    process_draw_string( &frame_string, &frame_string_size );
    printf("%s", frame_string);
    free(frame_string);
}

void routine(){
    while (1){
        render_frame();
        SLEEP(SLEEP_TIME);
	clear_console();
    }
}

void init(){
    //init

    atexit( cleanup );

    init_light_arrays();

    clear_frame_buffer();
    clear_depth_buffer();

    set_frustum_near_plane( 0.1 );
    set_frustum_far_plane( 1000 );
    set_frustum_FOV( 60.0 );


    RGB white = {255, 255, 255};
    RGB red = {255, 0, 0};

    add_ambient_light("Light", 10, white);

    Vec3 normal;
    normal.x = -2.0; normal.y = 0.0; normal.z = 1;
    add_directional_light("Some light", 100, vec3_normalize(normal), white);
    Vec3 pos;
    pos.x = 1; pos.y = 0; pos.z = 2;
    //add_point_light("Some point light", 150, 10, pos, white);

    set_player_position(0,0,0);
    set_player_rotation(0,0,0);
    
    clear_console();

}

int main(int argc, char* argv[]){

    //debug


    Texture* tex = load_texture("test2.bmp");

    //system("PAUSE");


    Model mdl = gen_model();

    mdl.position.z = 20.0;
    mdl.position.x = 0.0;
    mdl.scale.x = 1.0;
    mdl.scale.y = 1.0;
    mdl.scale.z = 1.0;
    //mdl.texture = tex;
    
    mptr = &mdl;



    load_model_obj_strategy("cube.obj", mptr);
    //system("PAUSE");

    /*for(size_t i = 0; i < mptr->mesh.usage; ++i)
	printf("%f %f %f\n", *((float*)mptr->mesh.buffer + i*3),
		*((float*)mptr->mesh.buffer + i*3 + 1),
		*((float*)mptr->mesh.buffer + i*3 + 2));

    printf("Mesh buffer usage: %d\n", mptr->mesh.usage);
    printf("Loaded\n");
    system("PAUSE");*/

    init();

    //Begin routine.
    routine();

    return 0;
}

