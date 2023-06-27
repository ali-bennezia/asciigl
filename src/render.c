/* 

	This file is part of asciigl.

	asciigl is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

	asciigl is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with asciigl. If not, see <https://www.gnu.org/licenses/>.

*/

#include "config.h"
#include "utils.h"
#include "render.h"
#include "state.h"

#include <stdio.h>


//graphics pipline functions

int is_viewspace_position_in_frustum(Vec3 pos, Vec2* clipSpace){

    //frustum center lign's locus
    Vec3 p = {0, 0, 0}, f = {0, 0, 1};

    //directions
    Vec3 player_lookright = {1, 0, 0}, player_lookup = {0, 1, 0};

    //factor for nearest point calculation
    float x = -( ( 3.0*(p.x*f.x + p.y * f.y + p.z * f.z) - 2.0*( f.x * pos.x + f.y * pos.y + f.z * pos.z ))/(2.0*( f.x*f.x + f.y*f.y + f.z*f.z )) );

    Vec3 nearest = { p.x + f.x*x, p.y + f.y*x, p.z + f.z*x };
    Vec3 difference = { pos.x - nearest.x, pos.y - nearest.y, pos.z - nearest.z };

    float frustum_plane_x_difference = vec3_division(difference, player_lookright);
    float frustum_plane_y_difference = vec3_division(difference, player_lookup);

    float r = tan( to_rads( get_frustum_FOV() / 2.0 ) );

    float frustum_plane_x_half_boundary = r*fabs( x );
    float frustum_plane_y_half_boundary = frustum_plane_x_half_boundary * (float)(FRAME_HEIGHT_WIDTH_RATIO);

    if (clipSpace != NULL){
	clipSpace->x = frustum_plane_x_half_boundary == 0 ? 0 : (frustum_plane_x_difference/frustum_plane_x_half_boundary);
	clipSpace->y = frustum_plane_y_half_boundary == 0 ? 0 : (frustum_plane_y_difference/frustum_plane_y_half_boundary);
    }

    int outsideNearOrFarPlane = ( x < get_frustum_near_plane() || x > get_frustum_far_plane() ) ? 1 : 0;
    int outsideFrustumBoundaries = ( fabs(frustum_plane_x_difference) > fabs(frustum_plane_x_half_boundary) || 
    fabs(frustum_plane_y_difference) > fabs(frustum_plane_y_half_boundary)) ? 1 : 0;

    return (outsideNearOrFarPlane == 1 || outsideFrustumBoundaries == 1) ? 0 : 1;
}

Vec3 worldspace_coords_to_viewspace_coords(Vec3 in){
    Vec3 out;

    Vec3 player_position = get_player_position(), player_rotation = get_player_rotation();
    Vec3 difference = {
    	in.x - player_position.x,
    	in.y - player_position.y,
    	in.z - player_position.z
    };

    double inToCameraDistance = vec3_magnitude( difference );

    double inToCameraXZAngleRadians = difference.z != 0 ? atan2(-difference.x, difference.z) : 0;
    double targetXZAngle = inToCameraXZAngleRadians;
    if (difference.z < 0)
	targetXZAngle = M_PI - targetXZAngle;
    targetXZAngle -= to_rads( get_player_rotation().y );

    double magnitudeXZ = sqrt( difference.x*difference.x + difference.z*difference.z );
    double inToCameraVerticalAngleRadians = magnitudeXZ != 0 ? atan2(difference.y, magnitudeXZ) : 0;
    double targetVerticalAngle = inToCameraVerticalAngleRadians - to_rads( player_rotation.x );

    double horizontalWeight = cos(targetVerticalAngle);
    double verticalWeight = sin(targetVerticalAngle);

    out.z = cos(targetXZAngle) * inToCameraDistance * horizontalWeight;
    out.y = verticalWeight * inToCameraDistance;
    out.x = -sin(targetXZAngle) * inToCameraDistance * horizontalWeight;

    return out;
}

Vec2 viewspace_coords_to_clipspace_coords(Vec3 in){
    Vec2 out;
    is_viewspace_position_in_frustum(in, &out);

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

Vec2 scale_normal_axis(float normal_x, float normal_y, float scale_x, float scale_y){

    Vec2 out;

    float normal_magnitude = sqrt( normal_x * normal_x + normal_y * normal_y );
    if (scale_x == 1 && scale_y == 1 || normal_magnitude == 0){
        out.x = normal_x; out.y = normal_y;
        return out;
    }

    float sin_theta = normal_magnitude == 0 ? 0 : (normal_y/normal_magnitude);
    float cos_theta = normal_magnitude == 0 ? 0 : (normal_x/normal_magnitude);

    float post_scale_radius = sqrt( scale_x * scale_x * cos_theta * cos_theta + scale_y * scale_y * sin_theta * sin_theta );

    if (post_scale_radius != 0){
    	out.x = (scale_y * cos_theta * normal_magnitude)/post_scale_radius;
    	out.y = (scale_x * sin_theta * normal_magnitude)/post_scale_radius;
    }else{
	out.x = 0; out.y = 0;
    }
    return out;
}

Vec3 scale_normal(Vec3 normal, Vec3 scale){

    Vec2 axis_normal, axis_scale;

    //X axis
    Vec2 x_axis_normal = scale_normal_axis( normal.z, normal.y, scale.z, scale.y );
    normal.z = x_axis_normal.x; normal.y = x_axis_normal.y;

    //Y axis
    Vec2 y_axis_normal = scale_normal_axis( normal.x, normal.z, scale.x, scale.z );
    normal.x = y_axis_normal.x; normal.z = y_axis_normal.y;

    //Z axis
    Vec2 z_axis_normal = scale_normal_axis( normal.x, normal.y, scale.x, scale.y );
    normal.x = z_axis_normal.x; normal.y = z_axis_normal.y;



    return normal;
}

void rasterize_segment(
	Segment longer_segment,
	Segment shorter_segment
){
	if ( !( is_segment_within_vertical_range( longer_segment, -1, 1 ) ) || !( is_segment_within_vertical_range( shorter_segment, -1, 1 ) ) )
		return;

	// Clamp within screen
	Segment clamped_shorter_segment;
	clamp_segment_within_vertical_range( &clamped_shorter_segment, shorter_segment, -1, 1);
	

	Vec2 pixel_size_clipspace = { 1.0/(float)FRAME_WIDTH, 1.0/(float)FRAME_HEIGHT };
	Vec2 longer_segment_vec2 = vec2_difference( longer_segment.end, longer_segment.start ), 
				clamped_shorter_segment_vec2 = vec2_difference( clamped_shorter_segment.end, clamped_shorter_segment.start );
	
	float clamped_shorter_segment_vertical_quotient = pixel_size_clipspace.y / clamped_shorter_segment_vec2.y;
	Vec2 clamped_shorter_segment_iteration_step = vec2_multiplication( clamped_shorter_segment_vec2, clamped_shorter_segment_vertical_quotient );

	Vec2 p = longer_segment.start, f = vec2_normalize( longer_segment_vec2 );

	size_t vertical_iterations = clamped_shorter_segment_vec2.y / pixel_size_clipspace.y;
	Vec2 pos = clamped_shorter_segment.start;
	for (size_t vertical_iteration = 0; vertical_iteration < vertical_iterations; ++vertical_iteration){
		pos = vec2_add( pos, pixel_size_clipspace );
		float x = -( ( 3.0*(p.x*f.x + p.y * f.y) - 2.0*( f.x * pos.x + f.y * pos.y ))/(2.0*( f.x*f.x + f.y*f.y )) );
		Vec2 nearest_longer_segment_point_clipspace = vec2_add( p, vec2_multiplication(f, x) );
		
		Segment horizontal_draw_segment	= {
			pos,
			nearest_longer_segment_point_clipspace
		}, clamped_horizontal_draw_segment;

		clamp_segment_within_horizontal_range( &clamped_horizontal_draw_segment, horizontal_draw_segment, -1, 1);
	}
}

void rasterize_and_draw_primitive_v2(
	Vec3 a_viewspace,
	Vec3 b_viewspace,
	Vec3 c_viewspace,

	Vec3* normals,
	Vec2* UVs,
	Texture* tex,
	Model* mdl
){

	Vec2 a_clipspace = viewspace_coords_to_clipspace_coords( a_viewspace ),
		b_clipspace = viewspace_coords_to_clipspace_coords( b_viewspace ),
		c_clipspace = viewspace_coords_to_clipspace_coords( c_viewspace );

	Vec2 ab_clipspace = vec2_difference( b_clipspace, a_clipspace ),
		bc_clipspace = vec2_difference( c_clipspace, b_clipspace ),
		ca_clipspace = vec2_difference( a_clipspace, c_clipspace );

	float ab_clipspace_length = vec2_magnitude( ab_clipspace ),
		bc_clipspace_length = vec2_magnitude( bc_clipspace ),
		ca_clipspace_length = vec2_magnitude( ca_clipspace );

	if ( ab_clipspace_length == 0 || bc_clipspace_length == 0 || ca_clipspace_length == 0 )
		return;

	float a_depth = vec3_magnitude( a_viewspace ),
		b_depth = vec3_magnitude( b_viewspace ),
		c_depth = vec3_magnitude( c_viewspace );

	/*Vec2 a_screenspace = vec2_int_to_float( clipspace_to_screenspace( a_clipspace ) ),
		b_screenspace = vec2_int_to_float( clipspace_to_screenspace( b_clipspace ) ),
		c_screenspace = vec2_int_to_float( clipspace_to_screenspace( c_clipspace ) );*/

	float ab_clipspace_vertical_span = fabs( ab_clipspace.y ),
		bc_clipspace_vertical_span = fabs( bc_clipspace.y ),
		ca_clipspace_vertical_span = fabs( ca_clipspace.y );

	Vec2* longest_segment_clipspace = NULL, *longest_segment_point1_clipspace = NULL,
		*short1_segment_clipspace = NULL, *short1_segment_point1_clipspace = NULL,	
		*short2_segment_clipspace = NULL, *short2_segment_point1_clipspace = NULL;

	if ( ab_clipspace_vertical_span >= bc_clipspace_vertical_span && ab_clipspace_vertical_span >= ca_clipspace_vertical_span ){

		longest_segment_clipspace = &ab_clipspace; longest_segment_point1_clipspace = &a_clipspace;

		short1_segment_clipspace = &bc_clipspace; short1_segment_point1_clipspace = &b_clipspace;
		short2_segment_clipspace = &ca_clipspace; short2_segment_point1_clipspace = &c_clipspace;

	}else if ( bc_clipspace_vertical_span >= ab_clipspace_vertical_span && bc_clipspace_vertical_span >=  ca_clipspace_vertical_span ){

		longest_segment_clipspace = &bc_clipspace; longest_segment_point1_clipspace = &b_clipspace;

		short1_segment_clipspace = &ab_clipspace; short1_segment_point1_clipspace = &a_clipspace;
		short2_segment_clipspace = &ca_clipspace; short2_segment_point1_clipspace = &c_clipspace;

	}else{

		longest_segment_clipspace = &ca_clipspace; longest_segment_point1_clipspace = &c_clipspace;

		short1_segment_clipspace = &ab_clipspace; short1_segment_point1_clipspace = &a_clipspace;
		short2_segment_clipspace = &bc_clipspace; short2_segment_point1_clipspace = &b_clipspace;

	}

	


}

//three viewspace positions describing a triangle primitive
void rasterize_and_draw_primitive(Vec3 a, Vec3 b, Vec3 c, Vec3* normals, Vec2* UVs, Texture* tex, Model* mdl){

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
    IntVec2* current_processed_segment_point1_screenspace = &short1_point1_screenspace;
    Vec2* current_processed_segment_clipspace = short1_clipspace;
    Vec2* current_processed_segment_point1_clipspace = short1_point1_clipspace;
    
    int done_short1 = 0;

    rasterize:;

    if (current_processed_segment_screenspace->y != 0){
	int vertical_rasterize = current_processed_segment_screenspace->y;
	
	int vertical_rasterize_point1_screenspace = current_processed_segment_point1_screenspace->y; 
	int vertical_rasterize_point2_screenspace = current_processed_segment_point1_screenspace->y + current_processed_segment_screenspace->y; 

	if ( (vertical_rasterize_point1_screenspace < 0 && vertical_rasterize_point2_screenspace < 0) ||
		(vertical_rasterize_point1_screenspace > FRAME_HEIGHT && vertical_rasterize_point2_screenspace > FRAME_HEIGHT) )
		goto pass;

	int vertical_rasterize_point1_screenspace_bounded = min(FRAME_HEIGHT, max(0, vertical_rasterize_point1_screenspace)); 
	int vertical_rasterize_point2_screenspace_bounded = min(FRAME_HEIGHT, max(0, vertical_rasterize_point2_screenspace));

	int vertical_iteration_begin_val = min( vertical_rasterize_point1_screenspace, vertical_rasterize_point2_screenspace ); 
	int vertical_iteration_end_val = max( vertical_rasterize_point1_screenspace, vertical_rasterize_point2_screenspace ); 

	int vertical_iteration_begin_val_bounded = min( vertical_rasterize_point1_screenspace_bounded, vertical_rasterize_point2_screenspace_bounded ); 
	int vertical_iteration_end_val_bounded = max( vertical_rasterize_point1_screenspace_bounded, vertical_rasterize_point2_screenspace_bounded ); 

	int y_begin_offset = vertical_iteration_begin_val_bounded - vertical_iteration_begin_val;  //vertical_rasterize_point1_screenspace - current_processed_segment_point1_screenspace->y;
	int y_end_offset = vertical_iteration_end_val - vertical_iteration_end_val_bounded; // (current_processed_segment_point1_screenspace->y + current_processed_segment_screenspace->y) - vertical_rasterize_point2_screnspace;

        for (int y = min(0, vertical_rasterize) + y_begin_offset; y <= max(0, vertical_rasterize) - y_end_offset; ++y){
            float progression = (float)abs(y)/(float)(abs(vertical_rasterize));
            Vec2 current_draw_clipspace 
            	= vec2_add(*current_processed_segment_point1_clipspace, vec2_multiplication( *current_processed_segment_clipspace, progression ));

            float nearest_factor = (current_draw_clipspace.y-longest_point1_clipspace->y)/longest_clipspace->y;
            nearest_screenspace = clipspace_coords_to_screenspace_coords( vec2_add( *longest_point1_clipspace, vec2_multiplication(*longest_clipspace, nearest_factor) ) );
            IntVec2 current_draw_screenspace = clipspace_coords_to_screenspace_coords(current_draw_clipspace);

	    int horizontal_rasterize_point1_screenspace = current_draw_screenspace.x;
	    int horizontal_rasterize_point2_screenspace = nearest_screenspace.x;

	    if ( (horizontal_rasterize_point1_screenspace < 0 && horizontal_rasterize_point2_screenspace < 0) ||
		(horizontal_rasterize_point1_screenspace > FRAME_WIDTH && horizontal_rasterize_point2_screenspace > FRAME_WIDTH) )
		goto pass;

	    int horizontal_rasterize_point1_screenspace_bounded = min(FRAME_WIDTH, max(0, horizontal_rasterize_point1_screenspace));
	    int horizontal_rasterize_point2_screenspace_bounded = min(FRAME_WIDTH, max(0, horizontal_rasterize_point2_screenspace));

	    int horizontal_iteration_begin_val = min(horizontal_rasterize_point1_screenspace, horizontal_rasterize_point2_screenspace);
	    int horizontal_iteration_end_val = max(horizontal_rasterize_point1_screenspace, horizontal_rasterize_point2_screenspace);

	    int horizontal_iteration_begin_val_bounded = min(horizontal_rasterize_point1_screenspace_bounded, horizontal_rasterize_point2_screenspace_bounded);
	    int horizontal_iteration_end_val_bounded = max(horizontal_rasterize_point1_screenspace_bounded, horizontal_rasterize_point2_screenspace_bounded);

	    int x_begin_offset = horizontal_iteration_begin_val_bounded - horizontal_iteration_begin_val;
	    int x_end_offset = horizontal_iteration_end_val - horizontal_iteration_end_val_bounded;

            int horizontal_rasterize = horizontal_rasterize_point2_screenspace - horizontal_rasterize_point1_screenspace;

	    int x_begin = max(0, min(0, horizontal_rasterize) + x_begin_offset);
	    int x_end = min(FRAME_WIDTH, max(0, horizontal_rasterize) - x_end_offset);
	    
            for (int x = x_begin; x <= x_end; ++x){

                Vec2 draw_point_clipspace_float;
                draw_point_clipspace_float.x = current_draw_clipspace.x + (float)x * 2.0/(float)FRAME_WIDTH;
                draw_point_clipspace_float.y = current_draw_clipspace.y;
                TriangularCoordinates coords = calculate_triangular_coordinates(a_clipspace, b_clipspace, c_clipspace, draw_point_clipspace_float);
                
                //fragment data
                Vec3 viewspace_position = vec3_add( vec3_multiplication(a, coords.a_weight), 
                    vec3_add(   vec3_multiplication(b, coords.b_weight), 
                                vec3_multiplication(c, coords.c_weight)));
		if ( is_viewspace_position_in_frustum( viewspace_position, NULL ) == 0 ) 
			continue;

                float depth = a_depth * coords.a_weight + b_depth * coords.b_weight + c_depth * coords.c_weight;
                Vec3 normal = normals != NULL ? vec3_add( vec3_multiplication(*(Vec3*)normals, coords.a_weight), 
                    vec3_add(   vec3_multiplication(*((Vec3*)normals + 1), coords.b_weight), 
                                vec3_multiplication(*((Vec3*)normals + 2), coords.c_weight))) : normal;
		Vec2 UV = UVs != NULL ? vec2_add( vec2_multiplication( *(Vec2*)UVs, coords.a_weight ),
				vec2_add( vec2_multiplication( *((Vec2*)UVs + 1), coords.b_weight ), vec2_multiplication( *((Vec2*)UVs + 2), coords.c_weight ) )) : UV;

		IntVec2 frag_screenspace = { 	current_draw_screenspace.x + x,
						clipspace_coords_to_screenspace_coords(*current_processed_segment_point1_clipspace).y + y };

                draw_fragment( 	frag_screenspace.x, 
				frag_screenspace.y, 
				depth,
				viewspace_position, 
				normals != NULL ? &normal : NULL,
				UVs != NULL ? &UV : NULL,
				tex,
				mdl );
            }

        }
    }
	pass:;

    if (done_short1 == 0){
        done_short1 = 1;

        current_processed_segment_screenspace = &short2_screenspace;
	current_processed_segment_point1_screenspace = &short2_point1_screenspace;
        current_processed_segment_clipspace = short2_clipspace;
        current_processed_segment_point1_clipspace = short2_point1_clipspace;

        goto rasterize;
    }
    
}

void draw_model(Model model){

    //condition to take normals & UVs into account during rendering
    char normals = model.mesh.usage == model.normals.usage/3 ? 1 : 0;
    char UVs = model.mesh.usage == model.UVs.usage/3 && model.UVs.usage != 0 ? 1 : 0;
    Vec2 placeholder_UVs[3] = { 
	{1, 1},
	{1, 0},
	{0, 0} 
    };

    //per-primitive
    for (size_t i = 0; i < model.mesh.usage; ++i){
        Triangle primitive = *(Triangle*)get_data( &model.mesh, i, sizeof(Triangle) );

        //normals
        Vec3* normals_ptr = normals == 1 ? (Vec3*)get_data( &model.normals, i*3, sizeof(Vec3)) : NULL;
        Vec3 normals_out[3];
        if (normals_ptr != NULL){
            normals_out[0] = rotate_point_around_origin( rotate_point_around_origin( scale_normal( *(normals_ptr), model.scale ), vec3_mirror( get_player_rotation() )), model.rotation );
            normals_out[1] = rotate_point_around_origin( rotate_point_around_origin( scale_normal( *(normals_ptr + 1), model.scale ), vec3_mirror( get_player_rotation() )), model.rotation );
            normals_out[2] = rotate_point_around_origin( rotate_point_around_origin( scale_normal( *(normals_ptr + 2), model.scale ), vec3_mirror( get_player_rotation() )), model.rotation );
            normals_ptr = &normals_out[0];
        }

	//UVs
	Vec2* UVs_ptr = UVs == 1 ? (Vec2*)get_data( &model.UVs, i*3, sizeof(Vec2) ) : &placeholder_UVs[0];

        Vec3 a_modelspace_rotated = rotate_point_around_origin( vec3_scale( primitive.a, model.scale ), model.rotation);
        Vec3 b_modelspace_rotated = rotate_point_around_origin( vec3_scale( primitive.b, model.scale ), model.rotation);
        Vec3 c_modelspace_rotated = rotate_point_around_origin( vec3_scale( primitive.c, model.scale ), model.rotation);

        Vec3 a_worldspace = vec3_add(model.position, a_modelspace_rotated);
        Vec3 b_worldspace = vec3_add(model.position, b_modelspace_rotated);
        Vec3 c_worldspace = vec3_add(model.position, c_modelspace_rotated);

        Vec3 a_viewspace = worldspace_coords_to_viewspace_coords(a_worldspace);
        Vec3 b_viewspace = worldspace_coords_to_viewspace_coords(b_worldspace);
        Vec3 c_viewspace = worldspace_coords_to_viewspace_coords(c_worldspace);
        
        rasterize_and_draw_primitive(a_viewspace, b_viewspace, c_viewspace, normals_ptr, UVs_ptr, model.texture, &model);
    }

}

void draw_fragment(int x, int y, float depth, Vec3 viewspacePosition, Vec3* normal, Vec2* UV, Texture* tex, Model* mdl){
    float current_depth = get_depth_buffer_depth(x, y);
    
    char screenCoordsTest = (x < 0 || x >= FRAME_WIDTH || y < 0 || y >= FRAME_HEIGHT) ? 0 : 1;
    char depthTest = ( get_depth_testing_state() == DEPTH_TESTING_STATE_DISABLED || current_depth == 0 || current_depth > depth) ? 1 : 0;
    char frustumTest = is_viewspace_position_in_frustum(viewspacePosition, NULL);

    if (screenCoordsTest == 1 && depthTest == 1 && frustumTest == 1){
	
	set_color_buffer_color(x, y, get_draw_color());
        DynamicArray* ambientLights = get_ambient_lights(), *directionalLights = get_directional_lights(), *pointLights = get_point_lights();

        //light levels must be expressed between 0 and 255. very low light levels (inferior to 1) will not be rendered.
        unsigned short lightLevel_red = 0,
		lightLevel_green = 0,
		lightLevel_blue = 0;

        for (size_t i = 0; i < ambientLights->usage; ++i){
            AmbientLight* ambientLight = (AmbientLight*)ambientLights->buffer + i;

	    float intensity = (float)ambientLight->intensity;
            lightLevel_red += ((float)ambientLight->color.red/255.0)*intensity;
	    lightLevel_green += ((float)ambientLight->color.green/255.0)*intensity;
	    lightLevel_blue += ((float)ambientLight->color.blue/255.0)*intensity;
        }

        if (normal != NULL)
            for (size_t i = 0; i < directionalLights->usage; ++i){
                DirectionalLight* directionalLight = (DirectionalLight*)directionalLights->buffer + i;
                float l = fabs(fmax(0.0,  vec3_dot_product( worldspace_coords_to_viewspace_coords( directionalLight->normal ), *normal ) ));

		float intensity = (float)(l * directionalLight->intensity);
                lightLevel_red += ((float)directionalLight->color.red/255.0) * intensity;
                lightLevel_green += ((float)directionalLight->color.green/255.0) * intensity;
                lightLevel_blue += ((float)directionalLight->color.blue/255.0) * intensity;
            }

        for (size_t i = 0; i < pointLights->usage; ++i){
            PointLight* pointLight = (PointLight*)pointLights->buffer + i;
            if (pointLight->intensity < 1) continue;
            float distance = vec3_magnitude( vec3_difference( worldspace_coords_to_viewspace_coords( pointLight->position ), viewspacePosition ) );
            if (distance > pointLight->range) continue;
            float raw_range = sqrt( pointLight->intensity - 1 );
            float adjusted_distance = distance*(raw_range/pointLight->range);

	    float intensity = ((float)pointLight->intensity/(1.0 + adjusted_distance * adjusted_distance ));
            lightLevel_red += ((float)pointLight->color.red/255.0)*intensity;
            lightLevel_green += ((float)pointLight->color.green/255.0)*intensity;
            lightLevel_blue += ((float)pointLight->color.blue/255.0)*intensity;
        }

        lightLevel_red = max( min( lightLevel_red, 255 ), 0);
        lightLevel_green = max( min( lightLevel_green, 255 ), 0);
        lightLevel_blue = max( min( lightLevel_blue, 255 ), 0);

	RGB fragColor = (tex != NULL && UV != NULL) ? 
		RGBA_to_RGB( sample_texture( UV->x, UV->y, tex ) ) : 
		mdl->color;
	unsigned short combined_red = (float)fragColor.red * (float)lightLevel_red / 255.0,
		combined_green = (float)fragColor.green * (float)lightLevel_green / 255.0,
		combined_blue = (float)fragColor.blue * (float)lightLevel_blue / 255.0;

	const unsigned short sqtwofivefive = pow(255, 2);
	unsigned short lightLevel = (float)( sqrt( pow( combined_red, 2 ) + pow( combined_green, 2 ) + pow( combined_blue, 2 ) ) 
		/ (float)sqrt( sqtwofivefive*3 ) ) * 255;

	set_draw_color( combined_red, combined_green, combined_blue );	
        set_frame_buffer_fragment(x, y, light_level_to_fragment(lightLevel)  );
        set_depth_buffer_depth(x, y, depth);
    }
}

void clear_console(){
        system(CLEAR_CMD);
	set_default_draw_color();
	//strcpy(&cache_ansi_draw_code[0], "     \0");
}

RGBA sample_texture(float UV_x, float UV_y, const Texture* tex)
{
	RGBA out = {255, 255, 255, 255};

	if (tex == NULL || tex->data == NULL) return out;

	size_t x = (size_t)(UV_x*(tex->width-1)) % tex->width,
		y = (size_t)(UV_y*(tex->height-1)) % tex->height;

	uint32_t pixelData = *((uint32_t*)tex->data + y*tex->width + x);
	out.alpha = pixelData >> 24 & 255;
	out.red = pixelData >> 16 & 255;
	out.green = pixelData >> 8 & 255;
	out.blue = pixelData & 255;

	return out;
}

