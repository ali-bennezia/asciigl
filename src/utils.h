#ifndef UTILS_HEADERGUARD
#define UTILS_HEADERGUARD

#include <stdio.h>
#include <stdlib.h>

#define _USE_MATH_DEFINES
#include <math.h>

typedef struct DynamicArray{
    void* buffer;
    size_t usage;
    size_t size;
} DynamicArray;

DynamicArray gen_dynamic_array(size_t dataSize);
void insert_data(DynamicArray* arr, void* data, size_t dataSize);
void* get_data(DynamicArray* arr, size_t index, size_t dataSize);
void set_data(DynamicArray* arr, size_t index, void* data, size_t dataSize);
void remove_data(DynamicArray* arr, size_t index, size_t dataSize);
void free_dynamic_array(DynamicArray* arr);

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

typedef struct Model{
    DynamicArray mesh, normals;

    //Transform in worldspace
    Vec3 position, rotation, scale;
} Model;

typedef struct AmbientLight{
    char* identifier;
    unsigned short intensity;
} AmbientLight;

typedef struct DirectionalLight{
    char* identifier;
    unsigned short intensity;
    Vec3 normal;
} DirectionalLight;

typedef struct PointLight{
    char* identifier;
    unsigned short intensity;
    float range;
    Vec3 position;
} PointLight;

enum LIGHT_TYPE{
    LIGHT_TYPE_AMBIENT,
    LIGHT_TYPE_DIRECTIONAL,
    LIGHT_TYPE_POINT
};

float floatmod(float a, float b);
float to_rads(float degrees);
float to_degs(float rads);

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

float vec3_magnitude(Vec3 vec);
float vec3_dot_product(Vec3 a, Vec3 b);
Vec3 vec3_cross_product(Vec3 a, Vec3 b);
Vec3 vec3_normalize(Vec3 vec);
float vec3_division(Vec3 dividand, Vec3 divider);
Vec3 vec3_multiplication(Vec3 vec, float scalar);
Vec3 vec3_mirror(Vec3 vec);
Vec3 vec3_add(Vec3 a, Vec3 b);
Vec3 vec3_difference(Vec3 a, Vec3 b);
Vec3 vec3_scale(Vec3 vec, Vec3 scale);

Vec3 rotate_point_around_origin(Vec3 position, Vec3 rotation);
TriangularCoordinates calculate_triangular_coordinates(Vec2 a, Vec2 b, Vec2 c, Vec2 p);
Vec2 vec2_int_to_float(IntVec2 vec);
IntVec2 vec2_float_to_int(IntVec2 vec);

void insert_primitive(Model* mdl, Triangle primitive);
void insert_primitives(Model* mdl, float* primitives, size_t primitivesCount);
void insert_primitive_normals(Model* mdl, Vec3 normals[3]);
void insert_primitives_normals(Model* mdl, float* normals, size_t primitivesCount);

Model gen_model();
void free_model(Model mdl);

#endif