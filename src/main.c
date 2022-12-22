
#include "config.h"
#include "state.h"
#include "utils.h"
#include "render.h"


//process

Model* mptr;

void render_frame(){
    if (get_depth_testing_state() == DEPTH_TESTING_STATE_ENABLED)
        clear_depth_buffer();

    for (int i = 0; i < FRAME_HEIGHT; ++i){
        for (int w = 0; w < FRAME_WIDTH; ++w)
            get_frame_buffer()[i * (FRAME_WIDTH+1) + w] = ' ';
        get_frame_buffer()[i * (FRAME_WIDTH+1) + FRAME_WIDTH] = '\n';
    }
    get_frame_buffer()[TOTAL_FRAGMENTS_PER_FRAME] = '\0';

    (&(mptr->rotation))->y += 4.0;
    (&(mptr->rotation))->x += 4.0;
    (&(mptr->rotation))->z += 4.0;

    draw_model(*mptr);

    printf("%s", get_frame_buffer());
}

void routine(){
    while (1){
        render_frame();
        SLEEP(SLEEP_TIME);
        system(CLEAR_CMD);
    }
}

void init(){

    //init

    clear_depth_buffer();

    set_frustum_near_plane( 0.1 );
    set_frustum_far_plane( 1000 );
    set_frustum_FOV( 60.0 );

    set_light_direction(-2, 0, 1);

    set_player_position(0,0,0);
    set_player_rotation(0,0,0);
    
    system(CLEAR_CMD);

}

int main(int argc, char* argv[]){

    //debug

    Vec3 testa, testb;
    testa.z = 0; testb.z = 0;
    testa.x = 0; testa.y = 1.0;
    testb.x = 0; testb.y = 0.5;
    float res = vec3_division(testa, testb);
    printf("%f\n", res);

    system("PAUSE");

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
    insert_primitive(&mdl, trgl);
    insert_primitive(&mdl, trgl2);
    insert_primitive(&mdl, trgl3);
    insert_primitive(&mdl, trgl4);
    insert_primitive(&mdl, trgl5);
    insert_primitive(&mdl, trgl6);
    insert_primitive(&mdl, trgl7);
    insert_primitive(&mdl, trgl8);
    insert_primitive(&mdl, trgl9);
    insert_primitive(&mdl, trgl10);
    insert_primitive(&mdl, trgl11);
    insert_primitive(&mdl, trgl12);
    
    mptr = &mdl;

    init();

    //Begin routine.
    routine();

    return 0;
}

