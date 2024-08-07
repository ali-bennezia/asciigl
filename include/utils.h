/* 

	This file is part of asciigl.

	asciigl is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

	asciigl is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with asciigl. If not, see <https://www.gnu.org/licenses/>.

*/

#ifndef UTILS_HEADERGUARD
#define UTILS_HEADERGUARD

#include <stdlib.h>
#include <stdint.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef _WIN32
#include <windows.h>
#endif

typedef struct DynamicArray{
    void* buffer;
    size_t usage;
    size_t size;
} DynamicArray;

DynamicArray gen_dynamic_array(size_t dataSize);
void clear_dynamic_array( DynamicArray* arr, size_t dataSize );
void *insert_data(DynamicArray* arr, void* data, size_t dataSize);
void* get_data(DynamicArray* arr, size_t index, size_t dataSize);
void set_data(DynamicArray* arr, size_t index, void* data, size_t dataSize);
void remove_data(DynamicArray* arr, size_t index, size_t dataSize);
void free_dynamic_array(DynamicArray* arr);

typedef struct RGB {
	unsigned short red;
	unsigned short green;
	unsigned short blue;
} RGB;

typedef struct RGBA {
	unsigned short red;
	unsigned short green;
	unsigned short blue;
	unsigned short alpha;
} RGBA;

typedef struct UnitRGB {
	float red;
	float green;
	float blue;
} UnitRGB;

RGB RGBA_to_RGB( RGBA in );
UnitRGB scale_to_unit_RGB( RGB color );

typedef struct Vector2{
    float x;
    float y;
} Vec2;

typedef struct IntVector2{
    int x;
    int y;
} IntVec2;

typedef struct Vector3{
    float x;
    float y;
    float z;
} Vec3;

typedef struct Triangle{
    Vec3 a,b,c;
} Triangle;

typedef struct TriangularCoordinates{
    Vec2 a,b,c;
    float a_weight, b_weight, c_weight;
} TriangularCoordinates;

typedef struct Texture{
    size_t sizeInBytes, width, height;
    //Pixels in 4-byte RGBA format 
    uint32_t* data;
} Texture;

enum RenderRotationMode
{
	ASCIIGL_RENDER_ROTATION_MODE_STANDARD,
	ASCIIGL_RENDER_ROTATION_MODE_BILLBOARD
};

typedef struct Mesh {
	DynamicArray vertices, normals, UVs;
} Mesh;

typedef struct Model{

    //Transform in worldspace
    Vec3 position, rotation, scale;

    RGBA color;

    Mesh *mesh;
    Texture *texture;

    enum RenderRotationMode rotationMode;
} Model;

typedef struct UIText{
	char *text;
	IntVec2 position;
	RGB color;
	size_t layer, size_x, size_y;
} UIText;

typedef struct UIFrame{
	IntVec2 position;
	IntVec2 size;
	RGB color;
	size_t layer;
} UIFrame;

typedef struct UIImage{
	IntVec2 position;
	IntVec2 size;
	RGB color;
	size_t layer;
	Texture *texture;
} UIImage;

typedef struct AmbientLight{
    char *identifier;
    unsigned short intensity;
    RGB color;
} AmbientLight;

typedef struct DirectionalLight{
    char* identifier;
    unsigned short intensity;
    Vec3 normal;
    RGB color;
} DirectionalLight;

typedef struct PointLight{
    char* identifier;
    unsigned short intensity;
    float range;
    Vec3 position;
    RGB color;
} PointLight;

enum LIGHT_TYPE{
    LIGHT_TYPE_AMBIENT,
    LIGHT_TYPE_DIRECTIONAL,
    LIGHT_TYPE_POINT
};

// Defines a segment in 2D floating-point coordinates
typedef struct Segment {
    Vec2 start, end;
} Segment;

float floatmod(float a, float b);
float to_rads(float degrees);
float to_degs(float rads);
float clamp(float f, float min, float max);
short sgn( int val );

float vec2_magnitude(Vec2 vec);
float vec2_dot_product(Vec2 a, Vec2 b);
Vec2 vec2_normalize(Vec2 vec);
Vec2 vec2_mirror(Vec2 vec);
Vec2 vec2_add(Vec2 a, Vec2 b);
Vec2 vec2_multiplication(Vec2 vec, float scalar);
float vec2_division(Vec2 dividand, Vec2 divider);
Vec2 vec2x2_average(Vec2 a, Vec2 b);
Vec2 vec2x3_average(Vec2 a, Vec2 b, Vec2 c);
Vec2 vec2_difference(Vec2 a, Vec2 b);
Vec2 vec2_inverse(Vec2 vec);

float vec3_magnitude(Vec3 vec);
float vec3_dot_product(Vec3 a, Vec3 b);
Vec3 vec3_cross_product(Vec3 a, Vec3 b);
Vec3 vec3_normalize(Vec3 vec);
float vec3_division(Vec3 dividand, Vec3 divider);
Vec3 vec3_multiplication(Vec3 vec, float scalar);
Vec3 vec3_mirror(Vec3 vec);
Vec3 vec3_add(Vec3 a, Vec3 b);
Vec3 vec3x3_add(Vec3 a, Vec3 b, Vec3 c);
Vec3 vec3_difference(Vec3 a, Vec3 b);
Vec3 vec3_scale(Vec3 vec, Vec3 scale);

int is_segment_within_vertical_range( Segment segment, float limit_1, float limit_2 );
int is_segment_within_horizontal_range( Segment segment, float limit_1, float limit_2 );
int clamp_segment_within_vertical_range( Segment* out, Segment segment, float limit_1, float limit_2 );
int clamp_segment_within_horizontal_range( Segment* out, Segment segment, float limit_1, float limit_2 );
int clamp_segment_within_vertical_horizontal_ranges( Segment* out, Segment segment, float range_1_limit_1, float range_1_limit_2, float range_2_limit_1, float range_2_limit_2 );

Vec3 rotate_point_around_origin(Vec3 position, Vec3 rotation);
TriangularCoordinates calculate_triangular_coordinates(Vec2 a, Vec2 b, Vec2 c, Vec2 p);
Vec2 vec2_int_to_float(IntVec2 vec);
IntVec2 vec2_float_to_int(IntVec2 vec);
float compute_plane_angle( float pos_x, float pos_y );
Vec2 rotate_plane_position( float pos_x, float pos_y, float delta_theta_rads );
Vec3 get_lookat_euler_angles_rotation(Vec3 lookat);

void insert_primitive(Model* mdl, Triangle primitive);
void insert_primitives(Model* mdl, float* primitives, size_t primitivesCount);
void insert_primitive_normals(Model* mdl, Vec3 normals[3]);
void insert_primitives_normals(Model* mdl, float* normals, size_t primitivesCount);

Model* gen_model();
Model* gen_model_billboard();
void free_model(Model *mdl);

const char* get_ansi_console_color_code( unsigned short red, unsigned short green, unsigned short blue );

UIText* gen_ui_text( char *text, IntVec2 position, RGB color, size_t layer );
UIText *set_ui_text( UIText *dest, char *text );
void free_ui_text( UIText *txt );

UIFrame* gen_ui_frame( IntVec2 position, IntVec2 size, RGB color, size_t layer );
void free_ui_frame( UIFrame *frame );

UIImage* gen_ui_image( IntVec2 position, IntVec2 size, RGB color, size_t layer, Texture* tex );
void free_ui_image( UIImage *image );

char *get_path_extension_alloc( char *path );
char *strrpl( char *str, char to_replace, char replace_with );
char *convert_float_string_to_current_locale( char *str );
void get_text_dimensions( char *txt, size_t *size_x, size_t *size_y );

#endif
