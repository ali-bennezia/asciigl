/* 

	This file is part of asciigl.

	asciigl is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

	asciigl is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with asciigl. If not, see <https://www.gnu.org/licenses/>.

*/

#ifndef STATE_HEADERGUARD
#define STATE_HEADERGUARD

#include "utils.h"

char light_level_to_fragment(unsigned short lightLevel);

enum DEPTH_TESTING_STATE{
    DEPTH_TESTING_STATE_ENABLED,
    DEPTH_TESTING_STATE_DISABLED
};

Vec3 get_player_lookat();
Vec3 get_player_lookup();

void process_draw_string( char** out, size_t* outSize );

char* get_frame_buffer();
float* get_depth_buffer();

void set_frame_buffer_fragment(int x, int y, char frag);
char get_frame_buffer_fragment(int x, int y);

void clear_frame_buffer();

void set_depth_buffer_depth(int x, int y, float depth);
float get_depth_buffer_depth(int x, int y);

void set_color_buffer_color(int x, int y, RGB color);
RGB get_color_buffer_color(int x, int y);

void set_depth_testing_state(enum DEPTH_TESTING_STATE state);
enum DEPTH_TESTING_STATE get_depth_testing_state();

void clear_depth_buffer();

void set_default_draw_color();
void set_draw_color(unsigned short red, unsigned short green, unsigned short blue);
RGB get_draw_color();
void set_print_color(unsigned short red, unsigned short green, unsigned short blue);
void set_default_print_color();

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

void add_ambient_light(char* identifier, unsigned short intensity, RGB color);
void add_directional_light(char* identifier, unsigned short intensity, Vec3 normal, RGB color);
void add_point_light(char* identifier, unsigned short intensity, float range, Vec3 position, RGB color);

AmbientLight* get_ambient_light(char* identifier);
DirectionalLight* get_directional_light(char* identifier);
PointLight* get_point_light(char* identifier);

DynamicArray* get_ambient_lights();
DynamicArray* get_directional_lights();
DynamicArray* get_point_lights();

void remove_light(char* identifier, enum LIGHT_TYPE lightType);

void init_light_arrays();

enum ObjectType
{
	ASCIIGL_MODEL,
	ASCIIGL_UI,
	ASCIIGL_GIZMO
};

typedef struct Object
{
	void *ptr;
	enum ObjectType type;
} Object;

void init_workspace();

Object *register_object( void* obj, enum ObjectType type );
void unregister_object( size_t index );
void unregister_object_with_mdl( Model *mdl );
void iterate_workspace( void(*iterator_func)( Object* ) );
void clear_workspace();

#endif
