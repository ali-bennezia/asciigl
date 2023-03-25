/* 

	This file is part of asciigl.

	asciigl is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

	asciigl is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with asciigl. If not, see <https://www.gnu.org/licenses/>.

*/

#include "state.h"
#include "config.h"
#include "utils.h"
#include "render.h"

//state

char frame_buffer[TOTAL_FRAGMENTS_PER_FRAME+1];
float depth_buffer[TOTAL_FRAGMENTS_PER_FRAME];
RGB color_buffer[TOTAL_FRAGMENTS_PER_FRAME];

Vec3 player_position;
Vec3 player_rotation;

Vec3 player_lookat, player_lookup;

float frustumNearPlane, frustumFarPlane, frustumFOV;

//dynamic lights
DynamicArray ambientLights, 
    directionalLights, 
    pointLights;

//funcs

DynamicArray* get_ambient_lights() { return &ambientLights; }
DynamicArray* get_directional_lights() { return &directionalLights; }
DynamicArray* get_point_lights() { return &pointLights; }

const char fragments[] = ".`-:;*=enDMLXE$@";
char light_level_to_fragment(unsigned short lightLevel){
    size_t correspondingIndex = (size_t)( (float)lightLevel/255.0*(strlen(&fragments[0]) - 1) ); 
    return fragments[ correspondingIndex ];
}

enum DEPTH_TESTING_STATE depthState = DEPTH_TESTING_STATE_ENABLED;

Vec3 get_player_lookat(){
    return player_lookat;
}

Vec3 get_player_lookup(){
    return player_lookup;
}

void process_draw_string( char** out, size_t* outSize )
{
    char* data = malloc( TOTAL_FRAGMENTS_PER_FRAME*8 + FRAME_HEIGHT );

    size_t data_index = 7;
    RGB col = get_color_buffer_color(0, 0);
    strcpy( data, get_ansi_console_color_code( col.red, col.green, col.blue ) ); 
    for (size_t i = 1; i < TOTAL_FRAGMENTS_PER_FRAME; ++i)
    {
	int x = i % FRAME_WIDTH;
	int y = (i - x)/FRAME_WIDTH;

	RGB icol = get_color_buffer_color( x, y );
	
	if (memcmp( &icol, &col, sizeof(RGB) ) != 0)
	{
		col = icol;
		strcpy( data + data_index, get_ansi_console_color_code( col.red, col.blue, col.green ) );
		data_index += 7;
	}

	data[data_index] = get_frame_buffer_fragment( x, y );	
	++data_index;
	
	if (x == FRAME_WIDTH - 1)
	{
		data[data_index] = '\n';
		++data_index;
	}	
    }
    data[data_index] = '\0';
    ++data_index;
    data = realloc( data, data_index );
    *outSize = data_index;
    *out = data;
}

char* get_frame_buffer(){
    return &frame_buffer[0];
}

float* get_depth_buffer(){
    return &depth_buffer[0];
}

void set_frame_buffer_fragment(int x, int y, char frag){
    //top-left = x 0 y 0
    if (x < 0 || x >= FRAME_WIDTH || y < 0 || y >= FRAME_HEIGHT)
        return;
    frame_buffer[y*FRAME_WIDTH + x] = frag;
}

char get_frame_buffer_fragment(int x, int y)
{
     //top-left = x 0 y 0
    if (x < 0 || x >= FRAME_WIDTH || y < 0 || y >= FRAME_HEIGHT)
        return ' ';
    return frame_buffer[y*FRAME_WIDTH + x]; 
}

void clear_frame_buffer()
{
    for (size_t i = 0; i < TOTAL_FRAGMENTS_PER_FRAME; ++i)
	frame_buffer[i] = ' ';
    frame_buffer[TOTAL_FRAGMENTS_PER_FRAME] = '\0';
}

void set_depth_buffer_depth(int x, int y, float depth){
    //top-left = x 0 y 0
    if (x < 0 || x >= FRAME_WIDTH || y < 0 || y >= FRAME_HEIGHT)
        return;
    depth_buffer[y*(FRAME_WIDTH) + x] = depth == 0 ? depth : 1.0/depth;
}

float get_depth_buffer_depth(int x, int y){
    //top-left = x 0 y 0
    if (x < 0 || x >= FRAME_WIDTH || y < 0 || y >= FRAME_HEIGHT)
        return 0.0;
    float depth = depth_buffer[y*(FRAME_WIDTH) + x];
    return depth == 0 ? depth : 1.0/depth;
}

void set_color_buffer_color(int x, int y, RGB color){
    //top-left = x 0 y 0
    if (x < 0 || x >= FRAME_WIDTH || y < 0 || y >= FRAME_HEIGHT)
        return;
    color_buffer[y*(FRAME_WIDTH) + x] = color;
}

RGB get_color_buffer_color(int x, int y){
    //top-left = x 0 y 0
    if (x < 0 || x >= FRAME_WIDTH || y < 0 || y >= FRAME_HEIGHT)
    {
	RGB white = {255, 255, 255};
	return white;
    }
    return color_buffer[y*(FRAME_WIDTH) + x];
}

void set_depth_testing_state(enum DEPTH_TESTING_STATE state) {
    depthState = state;
}

enum DEPTH_TESTING_STATE get_depth_testing_state(){
    return depthState;
}

void clear_depth_buffer(){
    for (size_t i = 0; i < FRAME_WIDTH * FRAME_HEIGHT; ++i)
        depth_buffer[i] = 0;
}


static RGB drawcolor;
void set_default_draw_color()
{
	//printf("\033[0;0m");
	drawcolor.red = 255;
	drawcolor.green = 255;
	drawcolor.blue = 255;
}

//static char cache_ansi_draw_code[6] = "     \0";
void set_draw_color(unsigned short red, unsigned short green, unsigned short blue) //TODO: Check if ANSI is actually supported
{

	/*const char* drawcode = get_ansi_console_color_code( red, green, blue );
	if ( strcmp( &cache_ansi_draw_code[0], &drawcode[0] ) != 0 ){
		strcpy( &cache_ansi_draw_code[0], &drawcode[0]);
		printf("%s", drawcode);
	}*/

	drawcolor.red = red;
	drawcolor.green = green;
	drawcolor.blue = blue;
}

RGB get_draw_color()
{
	return drawcolor;
}

float get_frustum_FOV(){
    return frustumFOV;
}

float get_frustum_near_plane(){
    return frustumNearPlane;
}

float get_frustum_far_plane(){
    return frustumFarPlane;
}

float set_frustum_FOV(float val){
    frustumFOV = val;
}

float set_frustum_near_plane(float val){
    frustumNearPlane = val;
}

float set_frustum_far_plane(float val){
    frustumFarPlane = val;
}


Vec3 get_player_position(){
    return player_position;
}

void set_player_position(float x, float y, float z){
    player_position.x = x;
    player_position.y = y;
    player_position.z = z;
}

void translate_player(float x, float y, float z){
    player_position.x += x;
    player_position.y += y;
    player_position.z += z;
}

Vec3 get_player_rotation(){
    return player_rotation;
}

void set_player_rotation(float x, float y, float z){
    player_rotation.x = floatmod(x, 360);
    player_rotation.y = floatmod(y, 360);
    player_rotation.z = floatmod(z, 360);

    //refresh player look direction vectors
    float lookAtAngle = to_rads( player_rotation.y ) + M_PI/2.0;

    float plr_vert_rot = to_rads( player_rotation.x );
    float c = cos( plr_vert_rot ) ;
    float x_comp = cos( lookAtAngle );
    float z_comp = sin( lookAtAngle );

    player_lookat.x = x_comp*c;
    player_lookat.y = sin( plr_vert_rot);
    player_lookat.z = z_comp*c;

    float lookupAngle = plr_vert_rot + M_PI/2.0;
    float c2 = cos( lookupAngle ) ;

    player_lookup.x = x_comp*c2;
    player_lookup.y = sin( lookupAngle );
    player_lookup.z = z_comp*c2;

}

void rotate_player(float x, float y, float z){
    player_rotation.x = floatmod(player_rotation.x + x, 360);
    player_rotation.y = floatmod(player_rotation.y + y, 360);
    player_rotation.z = floatmod(player_rotation.z + z, 360);
}

char lightArraysInit=0;
void init_light_arrays(){
    if (lightArraysInit == 1) return;
    lightArraysInit = 1;

    ambientLights = gen_dynamic_array( sizeof(AmbientLight) );
    directionalLights = gen_dynamic_array( sizeof(DirectionalLight) );
    pointLights = gen_dynamic_array( sizeof(PointLight) );
}

void add_ambient_light(char* identifier, unsigned short intensity, RGB color){
    //copy & store identifier string
    size_t len = strlen(identifier);
    char* stored_identifier = malloc( sizeof(char) * (len + 1) );
    strcpy(stored_identifier, identifier);

    //create & store light data
    AmbientLight ambientLight;
    ambientLight.identifier = stored_identifier;
    ambientLight.intensity = intensity;
    ambientLight.color = color;
    insert_data(&ambientLights, &ambientLight, sizeof(AmbientLight));
}

void add_directional_light(char* identifier, unsigned short intensity, Vec3 normal, RGB color){
    //copy & store identifier string
    size_t len = strlen(identifier);
    char* stored_identifier = malloc( sizeof(char) * (len + 1) );
    strcpy(stored_identifier, identifier);

    //create & store light data
    DirectionalLight directionalLight;
    directionalLight.identifier = stored_identifier;
    directionalLight.intensity = intensity;
    directionalLight.normal = vec3_normalize(normal);
    directionalLight.color = color;
    insert_data(&directionalLights, &directionalLight, sizeof(DirectionalLight));
}

void add_point_light(char* identifier, unsigned short intensity, float range, Vec3 position, RGB color){
    //copy & store identifier string
    size_t len = strlen(identifier);
    char* stored_identifier = malloc( sizeof(char) * (len + 1) );
    strcpy(stored_identifier, identifier);

    //create & store light data
    PointLight pointLight;
    pointLight.identifier = stored_identifier;
    pointLight.intensity = intensity;
    pointLight.range = range;
    pointLight.position = position;
    pointLight.color = color;
    insert_data(&pointLights, &pointLight, sizeof(PointLight));
}


AmbientLight* get_ambient_light(char* identifier){
    for (size_t i = 0; i < ambientLights.usage; ++i){
        AmbientLight* iterator = (AmbientLight*)(ambientLights.buffer) + i;
        if (strcmp(iterator->identifier, identifier))
            return iterator;
    }
    return NULL;
}

DirectionalLight* get_directional_light(char* identifier){
    for (size_t i = 0; i < directionalLights.usage; ++i){
        DirectionalLight* iterator = (DirectionalLight*)(directionalLights.buffer) + i;
        if (strcmp(iterator->identifier, identifier))
            return iterator;
    }
    return NULL;
}

PointLight* get_point_light(char* identifier){
    for (size_t i = 0; i < pointLights.usage; ++i){
        PointLight* iterator = (PointLight*)(pointLights.buffer) + i;
        if (strcmp(iterator->identifier, identifier))
            return iterator;
    }
    return NULL;
}



void remove_light(char* identifier, enum LIGHT_TYPE lightType){
    DynamicArray* darr = NULL;
    void* buffer = NULL;
    size_t dataSizeInBytes = 0;
    size_t iteration_max = 0;

    switch(lightType){
        case LIGHT_TYPE_AMBIENT:
        buffer = ambientLights.buffer;
        dataSizeInBytes = sizeof(AmbientLight);
        iteration_max = ambientLights.usage;
        darr = &ambientLights;
        break;
        case LIGHT_TYPE_DIRECTIONAL:
        buffer = directionalLights.buffer;
        dataSizeInBytes = sizeof(DirectionalLight);
        iteration_max = directionalLights.usage;
        darr = &directionalLights;
        break;
        default:
        buffer = pointLights.buffer;
        dataSizeInBytes = sizeof(PointLight);
        iteration_max = pointLights.usage;
        darr = &pointLights;
        break;
    }

    for (size_t i = 0; i < iteration_max; ++i){
        char* iterator = ((char*)buffer) + i * dataSizeInBytes;
        char* storedIdentifier = 
            lightType == LIGHT_TYPE_AMBIENT ?  ((AmbientLight*)iterator)->identifier : 
            lightType == LIGHT_TYPE_DIRECTIONAL ? ((DirectionalLight*)iterator)->identifier : 
            ((PointLight*)iterator)->identifier;

        if (strcmp( identifier, storedIdentifier ))
        {
            free(storedIdentifier);
            remove_data(darr, i, dataSizeInBytes);
            return;
        }
    }
}

