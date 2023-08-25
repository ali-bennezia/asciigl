/* 

	This file is part of asciigl.

	asciigl is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

	asciigl is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with asciigl. If not, see <https://www.gnu.org/licenses/>.

*/

#include "./../include/utils.h"
#include "./../include/state.h"
#include "./../include/images.h"
#include "./../include/config.h"

#include <string.h>

//utils

float floatmod(float a, float b){
    return a - ((int)(a/b))*b;
}

//dataSize: data type size in bytes
DynamicArray gen_dynamic_array(size_t dataSize){
    DynamicArray out;
    out.buffer = malloc( dataSize*10 );
    out.size = 10;
    out.usage = 0;
    return out;
}

void clear_dynamic_array( DynamicArray* arr, size_t dataSize )
{
	arr->buffer = realloc( arr->buffer, dataSize*10 );
	arr->size = 10;
	arr->usage = 0;
}

void *insert_data(DynamicArray* arr, void* data, size_t dataSize)
{
    if (arr->usage == arr->size){
        arr->size *= 2;
        arr->buffer = realloc(arr->buffer, arr->size * dataSize);
    }

    void *target = memcpy( ((char*)arr->buffer) + arr->usage*dataSize, data, dataSize );
    ++arr->usage;
    return target;
}

void set_data(DynamicArray* arr, size_t index, void* data, size_t dataSize){
    memcpy( ((char*)arr->buffer + index*dataSize), data, dataSize );
}

void* get_data(DynamicArray* arr, size_t index, size_t dataSize){
    return (void*)(((char*)arr->buffer) + index*dataSize);
}

void remove_data(DynamicArray* arr, size_t index, size_t dataSize)
{
    if (index < 0 || index >= arr->usage)
        return;

    if (index < arr->usage-1)
        memcpy( ((char*)arr->buffer) + index*dataSize, ((char*)arr->buffer) + (index+1)*dataSize, (arr->usage-(index+1))*dataSize );

    --arr->usage;
}

void free_dynamic_array(DynamicArray* arr){
    free( arr->buffer );
    arr->usage = 0;
    arr->size = 0;
}

RGB RGBA_to_RGB( RGBA in )
{
	RGB out;
	//memcpy( &out, &in, sizeof(RGB) );
	out.red = in.red;	
	out.green = in.green;
	out.blue = in.blue;
	return out;	
}

UnitRGB scale_to_unit_RGB(RGB color)
{
	UnitRGB out;

	float m = max( color.red, max( color.green, color.blue ) );
	if (m == 0)
	{
		out.red = 0; out.green = 0; out.blue = 0;
		return out;
	}

	out.red = (float)color.red / m;
	out.green = (float)color.green / m;
	out.blue = (float)color.blue / m;

	return out;
}

float to_rads(float degrees){
    return degrees / 180.0 * M_PI;
}

float to_degs(float rads){
    return rads / M_PI * 180.0;
}

float clamp(float f, float min, float max)
{
    const float t = f < min ? min : f;
    return t > max ? max : t;
}

short sgn( int val )
{
	if ( val > 0 ) return 1;
	else if ( val < 0 ) return -1;
	else return 0;
}

float vec2_magnitude(Vec2 vec){
    return sqrt( vec.x * vec.x + vec.y * vec.y );
}

float vec2_dot_product(Vec2 a, Vec2 b){
    return a.x * b.x + a.y * b.y;
}

Vec2 vec2_normalize(Vec2 vec){
    Vec2 out;
    float magnitude = vec2_magnitude(vec);
    if (magnitude == 0)
    {
        out.x = 0;
        out.y = 0;
    }else{
        out.x = vec.x /= magnitude;
        out.y = vec.y /= magnitude;
    }
    return out;
}

Vec2 vec2_mirror(Vec2 vec){
    Vec2 out;
    out.x = vec.x * -1.0;
    out.y = vec.y * -1.0;
    return out;
}

Vec2 vec2_add(Vec2 a, Vec2 b){
    Vec2 out;
    out.x = a.x + b.x;
    out.y = a.y + b.y;
    return out;
}

Vec2 vec2_multiplication(Vec2 vec, float scalar){
    Vec2 out;
    out.x = vec.x * scalar;
    out.y = vec.y * scalar;
    return out;
}

float vec2_division(Vec2 dividand, Vec2 divider){
    Vec2 normalized_dividand = vec2_normalize(dividand), normalized_divider = vec2_normalize(divider);
    float parallelity = vec2_dot_product(normalized_dividand, normalized_divider);
    float divider_magnitude = vec2_magnitude(divider);
    if (parallelity == 0 || divider_magnitude == 0)
        return 0;

    return vec2_magnitude(dividand) * parallelity / divider_magnitude;
}

Vec2 vec2x2_average(Vec2 a, Vec2 b){
    Vec2 out;
    out.x = (a.x + b.x) / 2.0;
    out.y = (a.y + b.y) / 2.0;
    return out;
}

Vec2 vec2x3_average(Vec2 a, Vec2 b, Vec2 c){
    Vec2 out;
    out.x = (a.x + b.x + c.x) / 3.0;
    out.y = (a.y + b.y + c.y) / 3.0;
    return out;
}

Vec2 vec2_difference(Vec2 a, Vec2 b){
    Vec2 out;
    out.x = a.x - b.x;
    out.y = a.y - b.y;
    return out;
}

Vec2 vec2_inverse(Vec2 vec)
{
    return vec2_multiplication( vec, -1.0 );
}

float vec3_magnitude(Vec3 vec){
    return sqrt( vec.x * vec.x + vec.y * vec.y + vec.z * vec.z );
}

float vec3_dot_product(Vec3 a, Vec3 b){
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

Vec3 vec3_cross_product(Vec3 a, Vec3 b){
    Vec3 out;
    out.z = a.x*b.y - a.y*b.x;
    out.x = a.y*b.z - a.z*b.y;
    out.y = a.z*b.x - a.x*b.z;
    return out;
}

Vec3 vec3_normalize(Vec3 vec){
    float magnitude = vec3_magnitude(vec);
    if (magnitude == 0)
    {
        //vec = { 0, 0, 0 };
        return vec;
    }else{
        vec.x /= magnitude;
        vec.y /= magnitude;
        vec.z /= magnitude;
        return vec;
    }
}

float vec3_division(Vec3 dividand, Vec3 divider){
    Vec3 normalized_dividand = vec3_normalize(dividand), normalized_divider = vec3_normalize(divider);
    float parallelity = vec3_dot_product(normalized_dividand, normalized_divider);
    float divider_magnitude = vec3_magnitude(divider);
    if (divider_magnitude == 0)
        return 0;

    return vec3_magnitude(dividand) * parallelity / divider_magnitude;
}

Vec3 vec3_multiplication(Vec3 vec, float scalar)
{
    Vec3 out = { 
	vec.x * scalar,
	vec.y * scalar,
	vec.z * scalar 
    };
    return out;
}

Vec3 vec3_mirror(Vec3 vec){
    Vec3 out;
    out.x = -vec.x;
    out.y = -vec.y;
    out.z = -vec.z;
    return out;
}

Vec3 vec3_add(Vec3 a, Vec3 b){
    Vec3 out;
    out.x = a.x + b.x;
    out.y = a.y + b.y;
    out.z = a.z + b.z;
    return out;
}

Vec3 vec3x3_add(Vec3 a, Vec3 b, Vec3 c){
    Vec3 out;
    out.x = a.x + b.x + c.x;
    out.y = a.y + b.y + c.y;
    out.z = a.z + b.z + c.z;
    return out;
}

Vec3 vec3_difference(Vec3 a, Vec3 b){
    Vec3 out;
    out.x = a.x - b.x;
    out.y = a.y - b.y;
    out.z = a.z - b.z;
    return out;
}

Vec3 vec3_scale(Vec3 vec, Vec3 scale){
    vec.x *= scale.x;
    vec.y *= scale.y;
    vec.z *= scale.z;
    return vec;
}

int is_segment_within_vertical_range( Segment segment, float limit_1, float limit_2 )
{
   float lower = fmin( limit_1, limit_2 ), higher = fmax( limit_1, limit_2 );
   
   if ( (segment.start.y > higher && segment.end.y > higher) || (segment.start.y < lower && segment.end.y < lower) )
	return 0; else return 1;
}

int is_segment_within_horizontal_range( Segment segment, float limit_1, float limit_2 )
{
   float lower = fmin( limit_1, limit_2 ), higher = fmax( limit_1, limit_2 );
   
   if ( (segment.start.x > higher && segment.end.x > higher) || (segment.start.x < lower && segment.end.x < lower) )
	return 0; else return 1;
}

int clamp_segment_within_vertical_range( Segment* out, Segment segment, float limit_1, float limit_2 )
{
   float region_vertical_upper_boundary = fmax( limit_1, limit_2 ), region_vertical_lower_boundary = fmin( limit_1, limit_2 );

   Vec2 segment_vec2 = vec2_difference( segment.end, segment.start );
   Vec2 segment_dir = vec2_normalize( segment_vec2 );
   Vec2 segment_inverse_dir = vec2_inverse( segment_dir );
   float segment_length = vec2_magnitude( segment_vec2 );

   if ( segment_dir.y == 0 ){
	*out = segment;
	return 1;
   }

   float start_add_vertical_quotient = 0, end_add_vertical_quotient = 0;

   if ( segment.start.y > region_vertical_upper_boundary ){
	float vertical_difference = region_vertical_upper_boundary - segment.start.y;
	start_add_vertical_quotient = segment_dir.y != 0 ? (vertical_difference / segment_dir.y) : 0;
   }else if ( segment.start.y < region_vertical_lower_boundary ){
	float vertical_difference = region_vertical_lower_boundary - segment.start.y;
	start_add_vertical_quotient = segment_dir.y != 0 ? (vertical_difference / segment_dir.y) : 0;
   }

   start_add_vertical_quotient = fmin( segment_length, start_add_vertical_quotient ); 
   start_add_vertical_quotient = fmax( -segment_length, start_add_vertical_quotient ); 
 
   segment.start = vec2_add( segment.start, vec2_multiplication( segment_dir, start_add_vertical_quotient ) ); 

   if ( segment.end.y > region_vertical_upper_boundary ){
	float vertical_difference = region_vertical_upper_boundary - segment.end.y;
	end_add_vertical_quotient = segment_inverse_dir.y != 0 ? (vertical_difference / segment_inverse_dir.y) : 0;
   }else if ( segment.end.y < region_vertical_lower_boundary ){
	float vertical_difference = region_vertical_lower_boundary - segment.end.y;
	end_add_vertical_quotient = segment_inverse_dir.y != 0 ? (vertical_difference / segment_inverse_dir.y) : 0;
   }  

   end_add_vertical_quotient = fmin( segment_length, end_add_vertical_quotient );
   end_add_vertical_quotient = fmax( -segment_length, end_add_vertical_quotient );
 
   segment.end = vec2_add( segment.end, vec2_multiplication( segment_inverse_dir, end_add_vertical_quotient ) );

   *out = segment;
   return 0; 
}

int clamp_segment_within_horizontal_range( Segment* out, Segment segment, float limit_1, float limit_2 )
{
   float region_horizontal_upper_boundary = fmax( limit_1, limit_2 ), region_horizontal_lower_boundary = fmin( limit_1, limit_2 );

   Vec2 segment_vec2 = vec2_difference( segment.end, segment.start );
   Vec2 segment_dir = vec2_normalize( segment_vec2 );
   Vec2 segment_inverse_dir = vec2_inverse( segment_dir );
   float segment_length = vec2_magnitude( segment_vec2 );

   if ( segment_dir.x == 0 ){
	*out = segment;
	return 1;
   }

   float start_add_horizontal_quotient = 0, end_add_horizontal_quotient = 0;

   if ( segment.start.x > region_horizontal_upper_boundary ){
	float horizontal_difference = region_horizontal_upper_boundary - segment.start.x;
	start_add_horizontal_quotient = segment_dir.x != 0 ? (horizontal_difference / segment_dir.x) : 0;
   }else if ( segment.start.x < region_horizontal_lower_boundary ){
	float horizontal_difference = region_horizontal_lower_boundary - segment.start.x;
	start_add_horizontal_quotient = segment_dir.x != 0 ? (horizontal_difference / segment_dir.x) : 0;
   }

   start_add_horizontal_quotient = fmin( segment_length, start_add_horizontal_quotient ); 
   start_add_horizontal_quotient = fmax( -segment_length, start_add_horizontal_quotient ); 

   segment.start = vec2_add( segment.start, vec2_multiplication( segment_dir, start_add_horizontal_quotient ) ); 

   if ( segment.end.x > region_horizontal_upper_boundary ){
	float horizontal_difference = region_horizontal_upper_boundary - segment.end.x;
	end_add_horizontal_quotient = segment_inverse_dir.x != 0 ? (horizontal_difference / segment_inverse_dir.x) : 0;
   }else if ( segment.end.x < region_horizontal_lower_boundary ){
	float horizontal_difference = region_horizontal_lower_boundary - segment.end.x;
	end_add_horizontal_quotient = segment_inverse_dir.x != 0 ? (horizontal_difference / segment_inverse_dir.x) : 0;
   }  

   end_add_horizontal_quotient = fmin( segment_length, end_add_horizontal_quotient );
   end_add_horizontal_quotient = fmax( -segment_length, end_add_horizontal_quotient );
 
   segment.end = vec2_add( segment.end, vec2_multiplication( segment_inverse_dir, end_add_horizontal_quotient ) );

   *out = segment;
   return 0; 
}

int clamp_segment_within_vertical_horizontal_ranges( Segment* out, Segment segment, float range_1_limit_1, float range_1_limit_2, float range_2_limit_1, float range_2_limit_2 )
{
    clamp_segment_within_vertical_range( out, segment, range_1_limit_1, range_1_limit_2 );
    clamp_segment_within_horizontal_range( out, *out, range_2_limit_1, range_2_limit_2 );
}

Vec3 rotate_point_around_origin(Vec3 position, Vec3 rotation){

    float x_rads = to_rads(rotation.x), y_rads = to_rads(rotation.y), z_rads = to_rads(rotation.z);

    float rotation_x_sin = sin( x_rads );
    float rotation_x_cos = cos( x_rads );

    float rotation_y_sin = sin( y_rads );
    float rotation_y_cos = cos( y_rads );

    float rotation_z_sin = sin( z_rads );
    float rotation_z_cos = cos( z_rads );

    Vec3 out;

    float zy_plane_distance = sqrt(position.z * position.z + position.y * position.y);

    if (zy_plane_distance != 0){

        float zy_plane_initial_sin = position.y / zy_plane_distance;
        float zy_plane_initial_cos = position.z / zy_plane_distance;

        float zy_plane_new_sin = zy_plane_initial_sin * rotation_x_cos + rotation_x_sin * zy_plane_initial_cos;
        float zy_plane_new_cos = zy_plane_initial_cos * rotation_x_cos - rotation_x_sin * zy_plane_initial_sin;

        out.z = zy_plane_new_cos * zy_plane_distance;
        out.y = zy_plane_new_sin * zy_plane_distance;

    }else{
        out.z = 0;
        out.y = 0;
    }

    float xz_plane_distance = sqrt(position.x * position.x + out.z * out.z);

    if (xz_plane_distance != 0){
        float xz_plane_initial_sin = out.z / xz_plane_distance;
        float xz_plane_initial_cos = position.x / xz_plane_distance;

        float xz_plane_new_sin = xz_plane_initial_sin * rotation_y_cos + xz_plane_initial_cos * rotation_y_sin;
        float xz_plane_new_cos = xz_plane_initial_cos * rotation_y_cos - xz_plane_initial_sin * rotation_y_sin;

        out.x = xz_plane_new_cos * xz_plane_distance;
        out.z = xz_plane_new_sin * xz_plane_distance;
    }else{
        out.x = 0;
        out.z = 0;
    }

    float xy_plane_distance = sqrt(out.x * out.x + out.y * out.y);

    if (xy_plane_distance != 0){

        float xy_plane_initial_sin = out.y / xy_plane_distance;
        float xy_plane_initial_cos = out.x / xy_plane_distance;

        float xy_plane_new_sin = xy_plane_initial_sin * rotation_z_cos + xy_plane_initial_cos * rotation_z_sin;
        float xy_plane_new_cos = xy_plane_initial_cos * rotation_z_cos - xy_plane_initial_sin * rotation_z_sin;

        out.y = xy_plane_new_sin * xy_plane_distance;
        out.x = xy_plane_new_cos * xy_plane_distance;
    }else{
        out.y = 0;
        out.x = 0;
    }



    return out;

}

TriangularCoordinates calculate_triangular_coordinates(Vec2 a, Vec2 b, Vec2 c, Vec2 p){
    TriangularCoordinates out;

    out.a = a;
    out.b = b;
    out.c = c;

    float ab = vec2_magnitude( vec2_difference(b, a) );
    float bc = vec2_magnitude( vec2_difference(c, b) );
    float ac = vec2_magnitude( vec2_difference(c, a) );

    float pb = vec2_magnitude( vec2_difference(p, b) );
    float pc = vec2_magnitude( vec2_difference(p, c) );

    float beta = (bc == 0 || ab == 0) ? 0 : acos( clamp( (ac*ac-bc*bc-ab*ab)/(-2.0*bc*ab), -1, 1) );
    float gamma_prime = (pc == 0 || bc == 0) ? 0 : acos( clamp( (pb*pb-pc*pc-bc*bc)/(-2.0*pc*bc), -1, 1) );
    float alpha_prime = M_PI - beta - gamma_prime;

    float alpha_prime_sin = sin(alpha_prime);

    float double_crad_ibc = alpha_prime_sin == 0 ? 0 : (bc/alpha_prime_sin);
    float ib = double_crad_ibc*sin(gamma_prime);
    float ic = double_crad_ibc*sin(beta);

    float ab_weight = ic == 0 ? 0 : (pc/ic);

    out.a_weight =  ab == 0 ? 0 : (ib/ab);
    out.b_weight = 1.0 - out.a_weight;
    out.c_weight = 1.0 - ab_weight;

    out.a_weight *= ab_weight;
    out.b_weight *= ab_weight;

    return out;
}

Vec2 vec2_int_to_float(IntVec2 vec)
{
    Vec2 out;
    out.x = (float)vec.x;
    out.y = (float)vec.y;
    return out;
}

IntVec2 vec2_float_to_int(IntVec2 vec)
{
    IntVec2 out;
    out.x = (int)vec.x;
    out.y = (int)vec.y;
    return out;
}


void insert_primitive(Model* mdl, Triangle primitive){
    insert_data( &mdl->mesh, &primitive, sizeof(Triangle) );
}

//primitivesCount is the number of Triangles ie the amount of elements in the array 'primitives' divided by 9
void insert_primitives(Model* mdl, float* primitives, size_t primitivesCount)
{
    for (size_t i = 0; i < primitivesCount; ++i){
        Triangle primitive;
        primitive.a.x = primitives[i*9];
        primitive.a.y = primitives[i*9 + 1];
        primitive.a.z = primitives[i*9 + 2];
        
        primitive.b.x = primitives[i*9 + 3];
        primitive.b.y = primitives[i*9 + 4];
        primitive.b.z = primitives[i*9 + 5];

        primitive.c.x = primitives[i*9 + 6];
        primitive.c.y = primitives[i*9 + 7];
        primitive.c.z = primitives[i*9 + 8];

        insert_primitive(mdl, primitive);
    }

}

void insert_primitive_normals(Model* mdl, Vec3 normals[3]){
    insert_data( &mdl->normals, &normals[0], sizeof(Vec3) );
    insert_data( &mdl->normals, &normals[1], sizeof(Vec3) );
    insert_data( &mdl->normals, &normals[2], sizeof(Vec3) );
}

void insert_primitives_normals(Model* mdl, float* normals, size_t primitivesCount)
{
    for (size_t i = 0; i < primitivesCount; ++i){
        Vec3 out_normals[3];

        Vec3 normal1, normal2, normal3;

        normal1.x = normals[i*9];
        normal1.y = normals[i*9 + 1];
        normal1.z = normals[i*9 + 2];

        normal2.x = normals[i*9 + 3];
        normal2.y = normals[i*9 + 4];
        normal2.z = normals[i*9 + 5];

        normal3.x = normals[i*9 + 6];
        normal3.y = normals[i*9 + 7];
        normal3.z = normals[i*9 + 8];

        out_normals[0] = normal1; out_normals[1] = normal2; out_normals[3] = normal3;

        insert_primitive_normals(mdl, out_normals);
    }
}

Model* gen_model(){
    Model* mdl = malloc( sizeof( Model ) );
    
    Vec3 pos = {0, 0, 0}, 
	rot = {0, 0, 0}, 
	scale = {1, 1, 1};
    RGBA color = {255, 255, 255, 255};

    mdl->position = pos;
    mdl->rotation = rot;
    mdl->scale = scale;

    mdl->mesh = gen_dynamic_array( sizeof(Triangle) );
    mdl->normals = gen_dynamic_array( sizeof(Vec3) );
    mdl->UVs = gen_dynamic_array( sizeof(Vec2) );

    mdl->color = color;
    mdl->texture = NULL;

    mdl->rotationMode = ASCIIGL_ROTATION_MODE_STANDARD;

    return ( Model* ) ( ( register_object( mdl, ASCIIGL_OBJTYPE_MODEL ) )->ptr );
}

Model* gen_model_billboard(){
    Model* mdl = malloc( sizeof( Model ) );
    
    Vec3 pos = {0, 0, 0}, 
	rot = {0, 0, 0}, 
	scale = {1, 1, 1};
    RGBA color = {255, 255, 255, 255};

    mdl->position = pos;
    mdl->rotation = rot;
    mdl->scale = scale;

    mdl->mesh = gen_dynamic_array( sizeof(Triangle) );
    mdl->normals = gen_dynamic_array( sizeof(Vec3) );
    mdl->UVs = gen_dynamic_array( sizeof(Vec2) );

    mdl->color = color;
    mdl->texture = NULL;

    mdl->rotationMode = ASCIIGL_ROTATION_MODE_BILLBOARD;
    return ( Model* ) ( ( register_object( mdl, ASCIIGL_OBJTYPE_MODEL_BILLBOARD ) )->ptr );
}

void free_model(Model *mdl){

    unregister_object_with_ptr( mdl );

    free_dynamic_array(&mdl->mesh);
    free_dynamic_array(&mdl->normals);
    free_dynamic_array(&mdl->UVs);
    if (mdl->texture != NULL)
	free_texture(mdl->texture);
    free(mdl);
}



RGB ansicolors[16] = {
	{0,0,0}, //black
	{139,0,0}, //dark red
	{0,100,0}, //dark green
	{103,165,95}, //dark yell.
	{0,0,139}, //dark blue
	{139,0,139}, //dark mag.
	{0,139,139}, //dark cyan
	{211,211,211}, //light gray
	{169,169,169}, //dark gray
	{255,0,0}, //red
	{0,255,0}, //green
	{255,128,0}, //orange
	{0,0,255}, //blue
	{255,0,255}, //magenta
	{0,255,255}, //cyan
	{255,255,255} //white
};
const char ansicolorcodes[16][8] = {
	"\033[0;30m\0",
	"\033[0;31m\0",
	"\033[0;32m\0",
	"\033[0;33m\0",
	"\033[0;34m\0",
	"\033[0;35m\0",
	"\033[0;36m\0",
	"\033[0;37m\0",
	"\033[0;90m\0",
	"\033[0;91m\0",
	"\033[0;92m\0",
	"\033[0;93m\0",
	"\033[0;94m\0",
	"\033[0;95m\0",
	"\033[0;96m\0",
	"\033[0;97m\0"
}; 
const char* get_ansi_console_color_code( unsigned short red, unsigned short green, unsigned short blue )
{
	RGB in = {red, green, blue};

	//color quotient
	UnitRGB inMix = scale_to_unit_RGB( in );

	//initialize to white
	size_t nearest = 15;
	RGB nearestRGB = ansicolors[15];	
	UnitRGB nearestMix = scale_to_unit_RGB( nearestRGB );
	float mdist = sqrt( 
		pow( nearestMix.red - inMix.red, 2 ) +
		pow( nearestMix.green - inMix.green, 2 ) +
		pow( nearestMix.blue - inMix.blue, 2 )
	);

	for (size_t i = 0; i < 16; ++i){
		//color mix quotient
		UnitRGB iMix = scale_to_unit_RGB( ansicolors[i] );

		//color mix distance
		float idist = sqrt( 
			pow( iMix.red-inMix.red, 2 ) +
			pow( iMix.green-inMix.green, 2 ) +
			pow( iMix.blue-inMix.blue, 2 )
		);
		
		if (idist < mdist){
			nearest = i;
			mdist = idist;
		}
	}
	return &(ansicolorcodes[nearest][0]);	
}

UIText* gen_ui_text( char *text, IntVec2 position, RGB color, size_t layer )
{
	UIText *txt = malloc( sizeof( UIText ) );
	char *text_cpy = malloc( strlen( text ) + 1 );
	strcpy( text_cpy, text );
	txt->text = text_cpy;
	txt->position = position;
	txt->color = color;
	txt->layer = layer;

	return ( UIText* ) ( register_object( txt, ASCIIGL_OBJTYPE_UI_TEXT ) );	
}

UIText *set_ui_text( UIText *dest, char *text )
{
	dest->text = realloc( dest->text, strlen( text ) + 1 );
	strcpy( dest->text, text );
	return dest;
}

void free_ui_text( UIText *txt )
{
	unregister_object_with_ptr( txt );
	free( txt->text );
	free( txt );
}

UIFrame* gen_ui_frame( IntVec2 position, IntVec2 size, RGB color, size_t layer )
{
	UIFrame *frame = malloc( sizeof( UIFrame ) );
	frame->position = position;
	frame->size = size;
	frame->color = color;
	frame->layer = layer;

	return ( UIFrame* ) ( register_object( frame, ASCIIGL_OBJTYPE_UI_FRAME ) );	
}

void free_ui_frame( UIFrame *frame )
{
	unregister_object_with_ptr( frame );
	free( frame );
}

UIImage* gen_ui_image( IntVec2 position, IntVec2 size, RGB color, size_t layer, Texture* tex )
{
	UIImage *image = malloc( sizeof( UIImage ) );
	image->position = position;
	image->size = size;
	image->color = color;
	image->layer = layer;
	image->texture = tex;

	return ( UIImage* ) ( register_object( image, ASCIIGL_OBJTYPE_UI_IMAGE ) );	
}

void free_ui_image( UIImage *image )
{
	unregister_object_with_ptr( image );
	free( image );
}
