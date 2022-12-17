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

typedef struct Mesh{
    DynamicArray primitives;
} Mesh;

typedef struct Model{
    Mesh mesh;

    //Transform in worldspace
    Vec3 position, rotation;
} Model;

typedef struct AmbientLight{
    char* identifier;
    size_t intensity;
} AmbientLight;

typedef struct DirectionalLight{
    char* identifier;
    size_t intensity;
    Vec3 normal;
} DirectionalLight;

typedef struct PointLight{
    char* identifier;
    Vec3 position;
    size_t intensity;
} PointLight;



float floatmod(float a, float b);
float to_rads(float degrees);
float to_degs(float rads);

float vec2_magnitude(Vec2 vec);
Vec2 vec2_normalize(Vec2 vec);
Vec2 vec2_add(Vec2 a, Vec2 b);
Vec2 vec2_multiplication(Vec2 vec, float scalar);

float vec3_magnitude(Vec3 vec);
float vec3_dot_product(Vec3 a, Vec3 b);
Vec3 vec3_cross_product(Vec3 a, Vec3 b);
Vec3 vec3_normalize(Vec3 vec);
float vec3_division(Vec3 dividand, Vec3 divider);
Vec3 vec3_multiplication(Vec3 vec, float scalar);
Vec3 vec3_mirror(Vec3 vec);
Vec3 vec3_add(Vec3 a, Vec3 b);

Vec3 rotate_point_around_origin(Vec3 position, Vec3 rotation);
TriangularCoordinates calculate_triangular_coordinates(Vec2 a, Vec2 b, Vec2 c, Vec2 p);
Vec2 vec2_int_to_float(IntVec2 vec);
IntVec2 vec2_flot_to_int(IntVec2 vec);

void insert_primitive(Model* mdl, Triangle primitive);
Model gen_model();
void free_model(Model mdl);

#endif