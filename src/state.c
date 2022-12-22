
#include "state.h"
#include "config.h"
#include "utils.h"

//state

char frame_buffer[TOTAL_FRAGMENTS_PER_FRAME+1];
float depth_buffer[FRAME_HEIGHT * FRAME_WIDTH];

Vec3 player_position;
Vec3 player_rotation;

Vec3 player_lookat, player_lookup;

float frustumNearPlane, frustumFarPlane, frustumFOV;

enum DEPTH_TESTING_STATE depthState = DEPTH_TESTING_STATE_ENABLED;

Vec3 get_player_lookat(){
    return player_lookat;
}

Vec3 get_player_lookup(){
    return player_lookup;
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
    frame_buffer[y*(FRAME_WIDTH+1) + x] = frag;
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

void draw_fragment(int x, int y, float depth, char frag){
    float current_depth = get_depth_buffer_depth(x, y);
    if (depthState == DEPTH_TESTING_STATE_DISABLED || current_depth == -1.0 || current_depth > depth){
        set_frame_buffer_fragment(x, y, frag);
        set_depth_buffer_depth(x, y, depth);
    }
}

void set_depth_testing_state(enum DEPTH_TESTING_STATE state) {
    depthState = state;
}

enum DEPTH_TESTING_STATE get_depth_testing_state(){
    return depthState;
}

void clear_depth_buffer(){
    for (int i = 0; i < FRAME_WIDTH * FRAME_HEIGHT; ++i)
        depth_buffer[i] = -1.0;
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

DynamicArray ambientLights, directionalLights, pointLights;

void add_ambient_light(char* identifier, float intensity){
    //copy & store identifier string
    size_t len = strlen(identifier);
    char* stored_identifier = malloc( sizeof(char) * (len + 1) );
    strcpy(stored_identifier, identifier);

    //create & store light data
    AmbientLight ambientLight;
    ambientLight.identifier = stored_identifier;
    ambientLight.intensity = intensity;
    insert_data(&ambientLights, &ambientLight, sizeof(AmbientLight));
}

void add_directional_light(char* identifier, float intensity, Vec3 normal){
    //copy & store identifier string
    size_t len = strlen(identifier);
    char* stored_identifier = malloc( sizeof(char) * (len + 1) );
    strcpy(stored_identifier, identifier);

    //create & store light data
    DirectionalLight directionalLight;
    directionalLight.identifier = stored_identifier;
    directionalLight.intensity = intensity;
    directionalLight.normal = vec3_normalize(normal);
    insert_data(&directionalLights, &directionalLight, sizeof(DirectionalLight));
}

void add_point_light(char* identifier, float intensity, float range, Vec3 position){
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
    void* buffer = NULL;
    size_t dataSizeInBytes = 0;

    switch(lightType){
        case LIGHT_TYPE_AMBIENT:
        buffer = ambientLights.buffer;
        dataSizeInBytes = sizeof(AmbientLight);
        break;
        case LIGHT_TYPE_DIRECTIONAL:
        buffer = directionalLights.buffer;
        dataSizeInBytes = sizeof(DirectionalLight);
        break;
        default:
        buffer = pointLights.buffer;
        dataSizeInBytes = sizeof(PointLight);
        break;
    }

    for (size_t i = 0; i < ambientLights.usage; ++i){
        char* iterator = ((char*)ambientLights.buffer) + i * dataSizeInBytes;
        char* storedIdentifier = 
            lightType == LIGHT_TYPE_AMBIENT ?  ((AmbientLight*)iterator)->identifier : 
            lightType == LIGHT_TYPE_DIRECTIONAL ? ((DirectionalLight*)iterator)->identifier : 
            ((PointLight*)iterator)->identifier;

        if (strcmp( identifier, storedIdentifier ))
        {
            free(storedIdentifier);
            remove_data(&ambientLights, i, sizeof(AmbientLight));
            return;
        }
    }
}


Vec3 light_direction;

void set_light_direction(float x, float y, float z){
    light_direction.x = x;
    light_direction.y = y;
    light_direction.z = z;
    light_direction = vec3_normalize(light_direction);
}

