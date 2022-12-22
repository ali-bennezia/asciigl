#include "config.h"
#include "utils.h"
#include "render.h"
#include "state.h"


//graphics pipline functions

int is_viewspace_position_in_frustum(Vec3 pos, Vec2* clipSpace){

    //frustum center lign's locus
    Vec3 p, f;

    p.x = 0;
    p.y = 0;
    p.z = 0;

    f.x = 0;
    f.y = 0;
    f.z = 1;

    //factor for nearest point calculation
    float x = -( ( 3.0*(p.x*f.x + p.y * f.y + p.z * f.z) - 2.0*( f.x * pos.x + f.y * pos.y + f.z * pos.z ))/(2.0*( f.x*f.x + f.y*f.y + f.z*f.z )) );

    Vec3 nearest;
    nearest.x = p.x + f.x*x;
    nearest.y = p.y + f.y*x;
    nearest.z = p.z + f.z*x;

    Vec3 difference;
    difference.x = pos.x - nearest.x;
    difference.y = pos.y - nearest.y;
    difference.z = pos.z - nearest.z;
    
    Vec3 player_lookright = vec3_normalize( vec3_cross_product(get_player_lookup(), f) );

    float frustum_plane_x_difference = vec3_division(difference, player_lookright);
    float frustum_plane_y_difference = vec3_division(difference, get_player_lookup());

    float r = tan( to_rads( get_frustum_FOV() / 2.0 ) );

    float frustum_plane_x_half_boundary = r*x;
    float frustum_plane_y_half_boundary = frustum_plane_x_half_boundary * (float)(FRAME_HEIGHT_WIDTH_RATIO);

    if (clipSpace != NULL){
        if (frustum_plane_x_half_boundary == 0.0 || frustum_plane_y_half_boundary == 0.0)
        {
            clipSpace->x = 0;
            clipSpace->y = 0;
        }else{
            clipSpace->x = frustum_plane_x_difference/frustum_plane_x_half_boundary;
            clipSpace->y = frustum_plane_y_difference/frustum_plane_y_half_boundary;
        }
    }

    if ( x < get_frustum_near_plane() || x > get_frustum_far_plane() )
        return 0;

    if ( fabs(frustum_plane_x_difference) > fabs(frustum_plane_x_half_boundary) || 
    fabs(frustum_plane_y_difference) > fabs(frustum_plane_y_half_boundary))
        return 0;

    return 1;
}

Vec3 worldspace_coords_to_viewspace_coords(Vec3 in){
    Vec3 out;

    Vec3 difference;

    Vec3 player_position = get_player_position(), player_rotation = get_player_rotation();

    difference.x = in.x - player_position.x;
    difference.y = in.y - player_position.y;
    difference.z = in.z - player_position.z;

    double inToCameraDistance = vec3_magnitude( difference );

    double inToCameraXZAngleRadians = atan2(difference.z, difference.x);
    double targetXZAngle = inToCameraXZAngleRadians - to_rads( get_player_rotation().y );

    double inToCameraVerticalAngleRadians = atan2(difference.y, sqrt( difference.x*difference.x + difference.z*difference.z ));
    double targetVerticalAngle = inToCameraVerticalAngleRadians - to_rads( player_rotation.x );

    double horizontalWeight = cos(targetVerticalAngle);
    double verticalWeight = sin(targetVerticalAngle);

    out.x = cos(targetXZAngle) * inToCameraDistance * horizontalWeight;
    out.y = verticalWeight * inToCameraDistance;
    out.z = sin(targetXZAngle) * inToCameraDistance * horizontalWeight;

    return out;
}

Vec2 viewspace_coords_to_clipspace_coords(Vec3 in){
    Vec2 out;
    is_viewspace_position_in_frustum(in, &out);

    //printf("viewspace: %f %f %f clipspace: %f %f\n",in.x, in.y, in.z, out.x, out.y);

    return out;
}

IntVec2 clipspace_coords_to_screenspace_coords(Vec2 in){

    IntVec2 out;
    float half_width = (float)(FRAME_WIDTH-1) / 2.0;
    float half_height = (float)(FRAME_HEIGHT-1) / 2.0;

    out.x = (int)(half_width + half_width * in.x);
    out.y = (int)(half_height - half_height * in.y);

    return out;
}

IntVec2 clipspace_to_screenspace(Vec2 in){

    IntVec2 out;
    float half_width = (float)(FRAME_WIDTH-1) / 2.0;
    float half_height = (float)(FRAME_HEIGHT-1) / 2.0;

    out.x = in.x * half_width;
    out.y = in.y * -half_height;

    return out;
}

//three viewspace positions describing a triangle primitive
void rasterize_and_draw_primitive(Vec3 a, Vec3 b, Vec3 c){

    Vec2 a_clipspace = viewspace_coords_to_clipspace_coords(a);
    Vec2 b_clipspace = viewspace_coords_to_clipspace_coords(b);
    Vec2 c_clipspace = viewspace_coords_to_clipspace_coords(c);

    float a_depth = vec3_magnitude(a);
    float b_depth = vec3_magnitude(b);
    float c_depth = vec3_magnitude(c);

    Vec2 a_screenspace_float = vec2_int_to_float( clipspace_to_screenspace(a_clipspace) );
    Vec2 b_screenspace_float = vec2_int_to_float( clipspace_to_screenspace(b_clipspace) );
    Vec2 c_screenspace_float = vec2_int_to_float( clipspace_to_screenspace(c_clipspace) );
    
    Vec2 ab_clipspace;
    ab_clipspace.x = b_clipspace.x-a_clipspace.x;
    ab_clipspace.y = b_clipspace.y-a_clipspace.y;
    
    Vec2 bc_clipspace;
    bc_clipspace.x = c_clipspace.x-b_clipspace.x;
    bc_clipspace.y = c_clipspace.y-b_clipspace.y;

    Vec2 ac_clipspace;
    ac_clipspace.x = c_clipspace.x-a_clipspace.x;
    ac_clipspace.y = c_clipspace.y-a_clipspace.y;

    Vec2* longest_clipspace;
    Vec2* longest_point1_clipspace;

    Vec2* short1_clipspace;
    Vec2* short1_point1_clipspace;

    Vec2* short2_clipspace;
    Vec2* short2_point1_clipspace;

    if (fabs(ab_clipspace.y) > fabs(bc_clipspace.y) && fabs(ab_clipspace.y) > fabs(ac_clipspace.y)){
        longest_clipspace = &ab_clipspace;
        longest_point1_clipspace = &a_clipspace;
        short1_clipspace = &bc_clipspace;
        short1_point1_clipspace = &b_clipspace;
        short2_clipspace = &ac_clipspace;
        short2_point1_clipspace = &a_clipspace;
    }
    else if (fabs(bc_clipspace.y) > fabs(ab_clipspace.y) && fabs(bc_clipspace.y) > fabs(ac_clipspace.y)){
        longest_clipspace = &bc_clipspace;
        longest_point1_clipspace = &b_clipspace;
        short1_clipspace = &ab_clipspace;
        short1_point1_clipspace = &a_clipspace;
        short2_clipspace = &ac_clipspace;
        short2_point1_clipspace = &a_clipspace;
    }
    else {
        longest_clipspace = &ac_clipspace;
        longest_point1_clipspace = &a_clipspace;
        short1_clipspace = &ab_clipspace;
        short1_point1_clipspace = &a_clipspace;
        short2_clipspace = &bc_clipspace;
        short2_point1_clipspace = &b_clipspace;
    }

    IntVec2 longest_screenspace = clipspace_to_screenspace(*longest_clipspace);
    IntVec2 longest_point1_screenspace = clipspace_coords_to_screenspace_coords(*longest_point1_clipspace);
    IntVec2 short1_screenspace = clipspace_to_screenspace(*short1_clipspace);
    IntVec2 short1_point1_screenspace = clipspace_coords_to_screenspace_coords(*short1_point1_clipspace);
    IntVec2 short2_screenspace = clipspace_to_screenspace(*short2_clipspace);
    IntVec2 short2_point1_screenspace = clipspace_coords_to_screenspace_coords(*short2_point1_clipspace);

    IntVec2 nearest_screenspace;

    IntVec2* current_processed_segment_screenspace = &short1_screenspace;
    Vec2* current_processed_segment_clipspace = short1_clipspace;
    Vec2* current_processed_segment_point1_clipspace = short1_point1_clipspace;
    
    char done_short1 = 0;

    rasterize:

    if (current_processed_segment_screenspace->y != 0)
        for (int y = min(0, current_processed_segment_screenspace->y); y <= max(0,current_processed_segment_screenspace->y); ++y){
            float progression = (float)abs(y)/(float)(abs(current_processed_segment_screenspace->y));
            Vec2 current_draw_clipspace 
            = vec2_add(*current_processed_segment_point1_clipspace, vec2_multiplication( *current_processed_segment_clipspace, progression ));

            float nearest_factor = (current_draw_clipspace.y-longest_point1_clipspace->y)/longest_clipspace->y;
            nearest_screenspace = clipspace_coords_to_screenspace_coords( vec2_add( *longest_point1_clipspace, vec2_multiplication(*longest_clipspace, nearest_factor) ) );
            IntVec2 current_draw_screenspace = clipspace_coords_to_screenspace_coords(current_draw_clipspace);

            int horizontal_rasterize = nearest_screenspace.x - current_draw_screenspace.x;

            for (int x = min(0, horizontal_rasterize); x <= max(0, horizontal_rasterize); ++x){

                Vec2 draw_point_screenspace_float;
                draw_point_screenspace_float.x = (float)(current_draw_screenspace.x + x);
                draw_point_screenspace_float.y = (float)(clipspace_coords_to_screenspace_coords(*current_processed_segment_point1_clipspace).y + y);
                TriangularCoordinates coords = calculate_triangular_coordinates(a_screenspace_float, b_screenspace_float, c_screenspace_float, draw_point_screenspace_float);
                float depth = a_depth * coords.a_weight + b_depth * coords.b_weight + c_depth * coords.c_weight;

                draw_fragment(current_draw_screenspace.x + x, clipspace_coords_to_screenspace_coords(*current_processed_segment_point1_clipspace).y + y, depth, '@');
                //set_frame_buffer_fragment(draw_point_screenspace_float.x, draw_point_screenspace_float.y, '@');
            }

        }

    if (done_short1 == 0){
        done_short1 = 1;

        current_processed_segment_screenspace = &short2_screenspace;
        current_processed_segment_clipspace = short2_clipspace;
        current_processed_segment_point1_clipspace = short2_point1_clipspace;

        goto rasterize;
    }
    
}

void draw_model(Model model){

    for (size_t i = 0; i < model.mesh.primitives.usage; ++i){
        Triangle primitive = *(Triangle*)get_data( &model.mesh.primitives, i, sizeof(Triangle) );

        Vec3 a_modelspace_rotated = rotate_point_around_origin(primitive.a, model.rotation);
        Vec3 b_modelspace_rotated = rotate_point_around_origin(primitive.b, model.rotation);
        Vec3 c_modelspace_rotated = rotate_point_around_origin(primitive.c, model.rotation);


        Vec3 a_worldspace = vec3_add(model.position, a_modelspace_rotated);
        Vec3 b_worldspace = vec3_add(model.position, b_modelspace_rotated);
        Vec3 c_worldspace = vec3_add(model.position, c_modelspace_rotated);

        Vec3 a_viewspace = worldspace_coords_to_viewspace_coords(a_worldspace);
        Vec3 b_viewspace = worldspace_coords_to_viewspace_coords(b_worldspace);
        Vec3 c_viewspace = worldspace_coords_to_viewspace_coords(c_worldspace);

        
        rasterize_and_draw_primitive(a_viewspace, b_viewspace, c_viewspace);

    }

}