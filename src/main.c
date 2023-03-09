
#include "config.h"
#include "state.h"
#include "utils.h"
#include "render.h"
#include "images.h"

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
    normal.x = -2.0; normal.y = 0.0; normal.z = 0;
    add_directional_light("Some light", 50, vec3_normalize(normal), white);
    Vec3 pos;
    pos.x = 1; pos.y = 0; pos.z = 2;
    add_point_light("Some point light", 50, 1, pos, white);

    set_player_position(0,0,0);
    set_player_rotation(0,0,0);
    
    clear_console();

}

int main(int argc, char* argv[]){

    //debug

    Texture* tex = load_texture("test5.bmp");

    system("PAUSE");

    Vec3 normal;

    normal.x = 0; normal.y = 0; normal.z = -1;
    Vec3 normals1[3] = {normal, normal, normal};

    Triangle trgl;
    Vec3 a, b, c;
    a.x = -0.2; a.y = -0.2; a.z = -0.2;
    b.x = 0.2; b.y = -0.2; b.z = -0.2;
    c.x = 0.2; c.y = 0.2; c.z = -0.2;
    trgl.a = a; trgl.b = b; trgl.c = c;

    Triangle trgl2;
    a.x = -0.2; a.y = -0.2; a.z = -0.2;
    b.x = -0.2; b.y = 0.2; b.z = -0.2;
    c.x = 0.2; c.y = 0.2; c.z = -0.2;
    trgl2.a = a; trgl2.b = b; trgl2.c = c;

    normal.x = 0; normal.y = 0; normal.z = 1;
    Vec3 normals2[3] = {normal, normal, normal};

    Triangle trgl3;
    a.x = -0.2; a.y = -0.2; a.z = 0.2;
    b.x = 0.2; b.y = -0.2; b.z = 0.2;
    c.x = 0.2; c.y = 0.2; c.z = 0.2;
    trgl3.a = a; trgl3.b = b; trgl3.c = c;

    Triangle trgl4;
    a.x = -0.2; a.y = -0.2; a.z = 0.2;
    b.x = -0.2; b.y = 0.2; b.z = 0.2;
    c.x = 0.2; c.y = 0.2; c.z = 0.2;
    trgl4.a = a; trgl4.b = b; trgl4.c = c;

    normal.x = 1; normal.y = 0; normal.z = 0;
    Vec3 normals3[3] = {normal, normal, normal};

    Triangle trgl5;
    a.x = 0.2; a.y = -0.2; a.z = -0.2;
    b.x = 0.2; b.y = -0.2; b.z = 0.2;
    c.x = 0.2; c.y = 0.2; c.z = 0.2;
    trgl5.a = a; trgl5.b = b; trgl5.c = c;

    Triangle trgl6;
    a.x = 0.2; a.y = -0.2; a.z = -0.2;
    b.x = 0.2; b.y = 0.2; b.z = 0.2;
    c.x = 0.2; c.y = 0.2; c.z = -0.2;
    trgl6.a = a; trgl6.b = b; trgl6.c = c;

    normal.x = -1; normal.y = 0; normal.z = 0;
    Vec3 normals4[3] = {normal, normal, normal};

    Triangle trgl7;
    a.x = -0.2; a.y = -0.2; a.z = -0.2;
    b.x = -0.2; b.y = -0.2; b.z = 0.2;
    c.x = -0.2; c.y = 0.2; c.z = 0.2;
    trgl7.a = a; trgl7.b = b; trgl7.c = c;

    Triangle trgl8;
    a.x = -0.2; a.y = -0.2; a.z = -0.2;
    b.x = -0.2; b.y = 0.2; b.z = 0.2;
    c.x = -0.2; c.y = 0.2; c.z = -0.2;
    trgl8.a = a; trgl8.b = b; trgl8.c = c;

    normal.x = 0; normal.y = 1; normal.z = 0;
    Vec3 normals5[3] = {normal, normal, normal};

    Triangle trgl9;
    a.x = -0.2; a.y = 0.2; a.z = -0.2;
    b.x = 0.2; b.y = 0.2; b.z = -0.2;
    c.x = 0.2; c.y = 0.2; c.z = 0.2;
    trgl9.a = a; trgl9.b = b; trgl9.c = c;

    Triangle trgl10;
    a.x = -0.2; a.y = 0.2; a.z = -0.2;
    b.x = 0.2; b.y = 0.2; b.z = 0.2;
    c.x = -0.2; c.y = 0.2; c.z = 0.2;
    trgl10.a = a; trgl10.b = b; trgl10.c = c;

    normal.x = 0; normal.y = -1; normal.z = 0;
    Vec3 normals6[3] = {normal, normal, normal};

    Triangle trgl11;
    a.x = -0.2; a.y = -0.2; a.z = -0.2;
    b.x = 0.2; b.y = -0.2; b.z = -0.2;
    c.x = 0.2; c.y = -0.2; c.z = 0.2;
    trgl11.a = a; trgl11.b = b; trgl11.c = c;

    Triangle trgl12;
    a.x = -0.2; a.y = -0.2; a.z = -0.2;
    b.x = 0.2; b.y = -0.2; b.z = 0.2;
    c.x = -0.2; c.y = -0.2; c.z = 0.2;
    trgl12.a = a; trgl12.b = b; trgl12.c = c;

    Model mdl = gen_model();

    mdl.position.z = 2.0;
    mdl.position.x = 0.0;
    mdl.scale.x = 4.0;

    insert_primitive(&mdl, trgl);
    insert_primitive(&mdl, trgl2);

    insert_primitive_normals(&mdl, normals1);
    insert_primitive_normals(&mdl, normals1);

    insert_primitive(&mdl, trgl3);
    insert_primitive(&mdl, trgl4);

    insert_primitive_normals(&mdl, normals2);
    insert_primitive_normals(&mdl, normals2);

    insert_primitive(&mdl, trgl5);
    insert_primitive(&mdl, trgl6);

    insert_primitive_normals(&mdl, normals3);
    insert_primitive_normals(&mdl, normals3);

    insert_primitive(&mdl, trgl7);
    insert_primitive(&mdl, trgl8);

    insert_primitive_normals(&mdl, normals4);
    insert_primitive_normals(&mdl, normals4);

    insert_primitive(&mdl, trgl9);
    insert_primitive(&mdl, trgl10);

    insert_primitive_normals(&mdl, normals5);
    insert_primitive_normals(&mdl, normals5);

    insert_primitive(&mdl, trgl11);
    insert_primitive(&mdl, trgl12);

    insert_primitive_normals(&mdl, normals6);
    insert_primitive_normals(&mdl, normals6);
    
    mptr = &mdl;




    init();

    //Begin routine.
    routine();

    return 0;
}

