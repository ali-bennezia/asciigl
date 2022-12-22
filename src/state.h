#ifndef STATE_HEADERGUARD
#define STATE_HEADERGUARD

#include "utils.h"

enum DEPTH_TESTING_STATE{
    DEPTH_TESTING_STATE_ENABLED,
    DEPTH_TESTING_STATE_DISABLED
};

Vec3 get_player_lookat();
Vec3 get_player_lookup();

char* get_frame_buffer();
float* get_depth_buffer();

void set_frame_buffer_fragment(int x, int y, char frag);
void set_depth_buffer_depth(int x, int y, float depth);
float get_depth_buffer_depth(int x, int y);
void draw_fragment(int x, int y, float depth, char frag);

void set_depth_testing_state(enum DEPTH_TESTING_STATE state);
enum DEPTH_TESTING_STATE get_depth_testing_state();

void clear_depth_buffer();

float get_frustum_FOV();
float get_frustum_near_plane();
float get_frustum_far_plane();

float set_frustum_FOV(float val);
float set_frustum_near_plane(float val);
float set_frustum_far_plane(float val);

Vec3 get_player_position();
void set_player_position(float x, float y, float z);
void translate_player(float x, float y, float z);

Vec3 get_player_rotation();
void set_player_rotation(float x, float y, float z);
void rotate_player(float x, float y, float z);

void add_ambient_light(char* identifier, float intensity);
void add_directional_light(char* identifier, float intensity, Vec3 normal);
void add_point_light(char* identifier, float intensity, float range, Vec3 position);

AmbientLight* get_ambient_light(char* identifier);
DirectionalLight* get_directional_light(char* identifier);
PointLight* get_point_light(char* identifier);

void remove_light(char* identifier, enum LIGHT_TYPE lightType);

#endif