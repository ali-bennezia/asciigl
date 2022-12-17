#include "utils.h"

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

void insert_data(DynamicArray* arr, void* data, size_t dataSize)
{
    if (arr->usage == arr->size){
        arr->size *= 2;
        arr->buffer = realloc(arr->buffer, arr->size * dataSize);
    }

    memcpy( ((char*)arr->buffer) + arr->usage*dataSize, data, dataSize);
    ++arr->usage;
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

float to_rads(float degrees){
    return degrees / 180.0 * M_PI;
}

float to_degs(float rads){
    return rads / M_PI * 180.0;
}

float vec2_magnitude(Vec2 vec){
    return sqrt( vec.x * vec.x + vec.y * vec.y );
}

Vec2 vec2_normalize(Vec2 vec){
    float magnitude = vec2_magnitude(vec);
    if (magnitude == 0)
    {
        vec.x = 0;
        vec.y = 0;
        return vec;
    }else{
        vec.x /= magnitude;
        vec.y /= magnitude;
        return vec;
    }
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
        vec.x = 0;
        vec.y = 0;
        vec.z = 0;
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
    if (parallelity == 0 || divider_magnitude == 0)
        return 0;

    return vec3_magnitude(dividand) / divider_magnitude * parallelity;
}

Vec3 vec3_multiplication(Vec3 vec, float scalar)
{
    Vec3 out;
    out.x = vec.x * scalar;
    out.y = vec.y * scalar;
    out.z = vec.z * scalar;
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

    float ab = vec2_magnitude( vec2_add( a, vec2_multiplication(b, -1.0) ) );
    float bc = vec2_magnitude( vec2_add( b, vec2_multiplication(c, -1.0) ) );
    float ac = vec2_magnitude( vec2_add( a, vec2_multiplication(c, -1.0) ) );

    float bp = vec2_magnitude( vec2_add( b, vec2_multiplication(p, -1.0) ) );
    float cp = vec2_magnitude( vec2_add( c, vec2_multiplication(p, -1.0) ) );

    float beta = acos( (ac*ac - ab*ab - bc*bc)/(-2*ab*bc) );
    float gamma_prime = acos( (bp*bp - cp*cp - bc*bc)/(-2*cp*bc) );
    float alpha_prime = M_PI - beta - gamma_prime;
    
    float double_crad = bc/sin(alpha_prime);
    float ci = double_crad * sin(beta);

    float ab_c_weight = cp/ci;
    out.c_weight = 1.0 - ab_c_weight;

    float bi = double_crad * sin(gamma_prime);
    float a_c_weight = bi/ab;
    out.a_weight = a_c_weight * ab_c_weight;
    out.b_weight = (1.0 - a_c_weight) * ab_c_weight;

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
    insert_data( &mdl->mesh.primitives, &primitive, sizeof(Triangle) );
}

Model gen_model(){
    Model mdl;
    
    Vec3 pos, rot;
    pos.x = 0; pos.y = 0; pos.z = 0;
    rot.x = 0; rot.y = 0; rot.z = 0;

    mdl.position = pos;
    mdl.rotation = rot;

    mdl.mesh.primitives = gen_dynamic_array( sizeof(Triangle) );

    return mdl;
}

void free_model(Model mdl){
    free_dynamic_array(&mdl.mesh.primitives);
}
