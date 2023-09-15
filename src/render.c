/* 

	This file is part of asciigl.

	asciigl is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

	asciigl is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with asciigl. If not, see <https://www.gnu.org/licenses/>.

*/

#include "./../include/config.h"
#include "./../include/utils.h"
#include "./../include/render.h"
#include "./../include/state.h"

#include <stdio.h>
#include <string.h>

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

	Vec3 player_position = get_player_position(), player_rotation = get_player_rotation();

	Vec3 pos_delta = {
    		in.x - player_position.x,
    		in.y - player_position.y,
    		in.z - player_position.z
	};

	Vec2 z_axis_pos = rotate_plane_position( pos_delta.x, pos_delta.y, to_rads( -player_rotation.z ) );
	pos_delta.x = z_axis_pos.x; pos_delta.y = z_axis_pos.y;

	Vec2 y_axis_pos = rotate_plane_position( pos_delta.x, pos_delta.z, to_rads( -player_rotation.y ) );
	pos_delta.x = y_axis_pos.x; pos_delta.z = y_axis_pos.y;

	Vec2 x_axis_pos = rotate_plane_position( pos_delta.z, pos_delta.y, to_rads( -player_rotation.x ) );
	pos_delta.z = x_axis_pos.x; pos_delta.y = x_axis_pos.y;

	return pos_delta;

}

Vec2 viewspace_coords_to_clipspace_coords(Vec3 in){
    Vec2 out;
    is_viewspace_position_in_frustum(in, &out);

    return out;
}

IntVec2 clipspace_coords_to_screenspace_coords(Vec2 in){

    IntVec2 out;
    float half_width = (float)(FRAME_WIDTH) / 2.0 ;//(float)(FRAME_WIDTH-1) / 2.0;
    float half_height =  (float)(FRAME_HEIGHT) / 2.0 ; //(float)(FRAME_HEIGHT-1) / 2.0;

    out.x = (int)(half_width + half_width * in.x);
    out.y = (int)(half_height - half_height * in.y);

    return out;
}

Vec2 screenspace_coords_to_clipspace_coords(IntVec2 in)
{
    const Vec2 pixel_clipspace_size = { 2.0/(float)FRAME_WIDTH, 2.0/(float)FRAME_HEIGHT };
    Vec2 out = {
	(in.x * pixel_clipspace_size.x) - 1.0,
	( 2.0-(in.y * pixel_clipspace_size.y) ) - 1.0
    };
    return out;
}

IntVec2 clipspace_to_screenspace(Vec2 in){

    IntVec2 out;
    float half_width = (float)(FRAME_WIDTH) / 2.0;
    float half_height = (float)(FRAME_HEIGHT) / 2.0;

    out.x = in.x * half_width;
    out.y = in.y * -half_height;

    return out;
}

// internalRelativePosition maps ( 0, 0 ) to the upper left corner of the pixel
Vec2 get_pixel_at_clipspace_coords_interior_clipspace_point( Vec2 pixelPosition, Vec2 internalRelativePosition )
{
    Vec2 internalAddition = {
	( 2.0 / (float)FRAME_WIDTH ) * internalRelativePosition.x,
	-( 2.0 / (float)FRAME_HEIGHT ) * internalRelativePosition.y
    };
    return vec2_add( screenspace_coords_to_clipspace_coords( clipspace_coords_to_screenspace_coords( pixelPosition ) ), internalAddition );
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

void rasterize_segment_fragment(
	IntVec2 fragment_screenspace,
	Vec2 fragment_clipspace,

	Vec3 a_viewspace,
	Vec3 b_viewspace,
	Vec3 c_viewspace,

	Vec2 a_clipspace,
	Vec2 b_clipspace,
	Vec2 c_clipspace,

	float a_depth, float b_depth, float c_depth,

	Vec3* normals,
	Vec2* UVs,
	Texture* tex,
	Model* mdl
)
{
	TriangularCoordinates coords = calculate_triangular_coordinates(a_clipspace, b_clipspace, c_clipspace, fragment_clipspace);
                
	//fragment data
	Vec3 viewspace_position = vec3x3_add( 
		vec3_multiplication(a_viewspace, coords.a_weight), 
		vec3_multiplication(b_viewspace, coords.b_weight), 
		vec3_multiplication(c_viewspace, coords.c_weight)
	);
	if ( is_viewspace_position_in_frustum( viewspace_position, NULL ) == 0 ) 
		return;

        float depth = a_depth * coords.a_weight + b_depth * coords.b_weight + c_depth * coords.c_weight;
        Vec3 normal = normals != NULL ? 
		vec3x3_add( 
			vec3_multiplication(*((Vec3*)normals + 0), coords.a_weight), 
                	vec3_multiplication(*((Vec3*)normals + 1), coords.b_weight), 
                        vec3_multiplication(*((Vec3*)normals + 2), coords.c_weight)
		)
		 : normal;
	Vec2 UV = UVs != NULL ? vec2_add( vec2_multiplication( *(Vec2*)UVs, coords.a_weight ),
		vec2_add( vec2_multiplication( *((Vec2*)UVs + 1), coords.b_weight ), vec2_multiplication( *((Vec2*)UVs + 2), coords.c_weight ) )) : UV;

	draw_fragment(	fragment_screenspace.x,
			fragment_screenspace.y,
			depth,
			viewspace_position, 
			normals != NULL ? &normal : NULL,
			UVs != NULL ? &UV : NULL,
			tex,
			mdl );

}

void rasterize_segment(
	Segment longer_segment,
	Segment shorter_segment,

	Vec3 a_viewspace,
	Vec3 b_viewspace,
	Vec3 c_viewspace,

	Vec2 a_clipspace,
	Vec2 b_clipspace,
	Vec2 c_clipspace,

	float a_depth, float b_depth, float c_depth,

	Vec3* normals,
	Vec2* UVs,
	Texture* tex,
	Model* mdl
){
	if ( !( is_segment_within_vertical_range( shorter_segment, -1, 1 ) || is_segment_within_vertical_range( longer_segment, -1, 1 )) ||
		!( is_segment_within_horizontal_range( shorter_segment, -1, 1 ) || is_segment_within_horizontal_range( longer_segment, -1, 1 ) ) )
		return;

	// Clamp within screen
	Segment clamped_longer_segment, clamped_shorter_segment;
	clamp_segment_within_vertical_range( &clamped_longer_segment, longer_segment, -1, 1 );
	clamp_segment_within_vertical_range( &clamped_shorter_segment, shorter_segment, -1, 1 );

	const Vec2 pixel_size_clipspace = { 2.0/(float)FRAME_WIDTH, 2.0/(float)FRAME_HEIGHT };
	const Vec2 half_pixel_size_clipspace = vec2_multiplication( pixel_size_clipspace, 0.5 );

	Vec2 clamped_longer_segment_vec2 = vec2_difference( clamped_longer_segment.end, clamped_longer_segment.start ),
		clamped_shorter_segment_vec2 = vec2_difference( clamped_shorter_segment.end, clamped_shorter_segment.start );

	const float clamped_shorter_segment_vertical_quotient = clamped_shorter_segment_vec2.y != 0 ? fabs( pixel_size_clipspace.y / clamped_shorter_segment_vec2.y ) : 0;
	Vec2 clamped_shorter_segment_vertical_iteration_step = vec2_multiplication( clamped_shorter_segment_vec2, clamped_shorter_segment_vertical_quotient );

	const float clamped_shorter_segment_horizontal_quotient = clamped_shorter_segment_vec2.x != 0 ? fabs( pixel_size_clipspace.x / clamped_shorter_segment_vec2.x ) : 0;
	const Vec2 clamped_shorter_segment_horizontal_iteration_step = vec2_multiplication( clamped_shorter_segment_vec2, clamped_shorter_segment_horizontal_quotient );

	IntVec2 draw_pixel_position_screenspace = clipspace_coords_to_screenspace_coords( clamped_shorter_segment.start );
	int draw_pixel_vertical_step = clamped_shorter_segment_vertical_iteration_step.y < 0 ? 1 : -1;

	size_t vertical_iterations = abs( clipspace_to_screenspace( clamped_shorter_segment_vec2 ).y ) + 1;
	Vec2 pos = clamped_shorter_segment.start;

	Vec2 previous_pos = pos;

	for (size_t vertical_iteration = 0; vertical_iteration < vertical_iterations; ++vertical_iteration){

		// handle horizontal gaps
		if (vertical_iteration > 0){

			size_t gap_fills = max( abs( clipspace_to_screenspace( vec2_difference( pos, previous_pos ) ).x ), 0 );

			Vec2 fill_clipspace = vec2_add( previous_pos, clamped_shorter_segment_horizontal_iteration_step );
			for ( size_t gap_fill = 0; gap_fill < gap_fills; ++gap_fill ){
				IntVec2 fill_screenspace = clipspace_coords_to_screenspace_coords( fill_clipspace );
				rasterize_segment_fragment(
					fill_screenspace,
					fill_clipspace,

					a_viewspace,
					b_viewspace,
					c_viewspace,

					a_clipspace,
					b_clipspace,
					c_clipspace,

					a_depth, 
					b_depth, 
					c_depth,
		
					normals,
					UVs,
					tex,
					mdl
				);

				fill_clipspace = vec2_add( fill_clipspace, clamped_shorter_segment_horizontal_iteration_step );

			}	

		}

		// handle vertical iteration

	        float nearest_factor = clamped_longer_segment_vec2.y != 0 ? ( (pos.y-clamped_longer_segment.start.y)/clamped_longer_segment_vec2.y ) : 0;

        	Vec2 nearest_longer_segment_point_clipspace = vec2_add( clamped_longer_segment.start, vec2_multiplication(clamped_longer_segment_vec2, nearest_factor) );
		IntVec2 nearest_longer_segment_point_screenspace = clipspace_coords_to_screenspace_coords( nearest_longer_segment_point_clipspace );	

		Segment horizontal_draw_segment	= {
			pos,
			nearest_longer_segment_point_clipspace
		}, clamped_horizontal_draw_segment;

		clamp_segment_within_horizontal_range( &clamped_horizontal_draw_segment, horizontal_draw_segment, -1, 1 );
		Vec2 clamped_horizontal_draw_segment_vec2 = vec2_difference( clamped_horizontal_draw_segment.end, clamped_horizontal_draw_segment.start );

		draw_pixel_position_screenspace.x = clipspace_coords_to_screenspace_coords( pos ).x;
		size_t horizontal_iterations = min( 
			abs( nearest_longer_segment_point_screenspace.x - draw_pixel_position_screenspace.x ) + 1, 
			FRAME_WIDTH
		);
	
		int hpos_x_iteration_step_screenspace = clamped_horizontal_draw_segment_vec2.x >= 0 ? 1 : -1;
		float hpos_x_iteration_step_clipspace = clamped_horizontal_draw_segment_vec2.x >= 0 ? pixel_size_clipspace.x : -pixel_size_clipspace.x;

		Vec2 hpos = pos;
		IntVec2 hpos_screenspace = draw_pixel_position_screenspace;

		for (size_t horizontal_iteration = 0; horizontal_iteration < horizontal_iterations; ++horizontal_iteration){
			rasterize_segment_fragment(
				hpos_screenspace,
				hpos,

				a_viewspace,
				b_viewspace,
				c_viewspace,

				a_clipspace,
				b_clipspace,
				c_clipspace,

				a_depth, 
				b_depth, 
				c_depth,
		
				normals,
				UVs,
				tex,
				mdl
			);

			hpos_screenspace.x += hpos_x_iteration_step_screenspace;
			hpos.x += hpos_x_iteration_step_clipspace; 
		}

		previous_pos = pos;
		pos = vec2_add( pos, clamped_shorter_segment_vertical_iteration_step );
		draw_pixel_position_screenspace.y += draw_pixel_vertical_step;

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

	float ab_clipspace_vertical_span = fabs( ab_clipspace.y ),
		bc_clipspace_vertical_span = fabs( bc_clipspace.y ),
		ca_clipspace_vertical_span = fabs( ca_clipspace.y );

	Vec2* longest_segment_clipspace = NULL, *longest_segment_point1_clipspace = NULL,
		*short1_segment_clipspace = NULL, *short1_segment_point1_clipspace = NULL,	
		*short2_segment_clipspace = NULL, *short2_segment_point1_clipspace = NULL;

	if ( ab_clipspace_vertical_span > bc_clipspace_vertical_span && ab_clipspace_vertical_span > ca_clipspace_vertical_span ){

		longest_segment_clipspace = &ab_clipspace; longest_segment_point1_clipspace = &a_clipspace;

		short1_segment_clipspace = &bc_clipspace; short1_segment_point1_clipspace = &b_clipspace;
		short2_segment_clipspace = &ca_clipspace; short2_segment_point1_clipspace = &c_clipspace;

	}else if ( bc_clipspace_vertical_span > ab_clipspace_vertical_span && bc_clipspace_vertical_span > ca_clipspace_vertical_span ){

		longest_segment_clipspace = &bc_clipspace; longest_segment_point1_clipspace = &b_clipspace;

		short1_segment_clipspace = &ab_clipspace; short1_segment_point1_clipspace = &a_clipspace;
		short2_segment_clipspace = &ca_clipspace; short2_segment_point1_clipspace = &c_clipspace;

	}else{

		longest_segment_clipspace = &ca_clipspace; longest_segment_point1_clipspace = &c_clipspace;

		short1_segment_clipspace = &ab_clipspace; short1_segment_point1_clipspace = &a_clipspace;
		short2_segment_clipspace = &bc_clipspace; short2_segment_point1_clipspace = &b_clipspace;

	}

	Segment longer_segment = {
		*longest_segment_point1_clipspace,
		vec2_add( *longest_segment_point1_clipspace, *longest_segment_clipspace )
	}, short1_segment = {
		*short1_segment_point1_clipspace,
		vec2_add( *short1_segment_point1_clipspace, *short1_segment_clipspace )
	}, short2_segment = {
		*short2_segment_point1_clipspace,
		vec2_add( *short2_segment_point1_clipspace, *short2_segment_clipspace )
	};

	rasterize_segment(
		longer_segment,
		short1_segment,
		
		a_viewspace,
		b_viewspace,
		c_viewspace,

		a_clipspace,
		b_clipspace,
		c_clipspace,

		a_depth, b_depth, c_depth,

		normals,
		UVs,
		tex,
		mdl
	);

	rasterize_segment(
		longer_segment,
		short2_segment,
		
		a_viewspace,
		b_viewspace,
		c_viewspace,

		a_clipspace,
		b_clipspace,
		c_clipspace,

		a_depth, b_depth, c_depth,

		normals,
		UVs,
		tex,
		mdl
	);	

}

void draw_model(Model model){

    if ( !model.mesh ) return;

    //condition to take normals & UVs into account during rendering
    char normals = model.mesh->vertices.usage == model.mesh->normals.usage/3 ? 1 : 0;
    char UVs = model.mesh->vertices.usage == model.mesh->UVs.usage/3 && model.mesh->UVs.usage != 0 ? 1 : 0;
    Vec2 placeholder_UVs[3] = { 
	{1, 1},
	{1, 0},
	{0, 0} 
    };

    //per-primitive
    for (size_t i = 0; i < model.mesh->vertices.usage; ++i){
        Triangle primitive = *((Triangle*)get_data( &model.mesh->vertices, i, sizeof(Triangle) ));

        //normals
        Vec3* normals_ptr = normals == 1 ? (Vec3*)get_data( &model.mesh->normals, i*3, sizeof(Vec3) ) : NULL;
        Vec3 normals_out[3];
        if (normals_ptr != NULL){
            normals_out[0] = rotate_point_around_origin( scale_normal( *(normals_ptr), model.scale ), model.rotation );
            normals_out[1] = rotate_point_around_origin( scale_normal( *(normals_ptr + 1), model.scale ), model.rotation );
            normals_out[2] = rotate_point_around_origin( scale_normal( *(normals_ptr + 2), model.scale ), model.rotation );
            normals_ptr = &normals_out[0];
        }

	//UVs
	Vec2* UVs_ptr = UVs == 1 ? (Vec2*)get_data( &model.mesh->UVs, i*3, sizeof(Vec2) ) : &placeholder_UVs[0];

        Vec3 a_modelspace_rotated = vec3_scale( primitive.a, model.scale ); 
        Vec3 b_modelspace_rotated = vec3_scale( primitive.b, model.scale ); 
        Vec3 c_modelspace_rotated = vec3_scale( primitive.c, model.scale ); 

	if ( model.rotationMode == ASCIIGL_RENDER_ROTATION_MODE_BILLBOARD )
	{
		Vec3 cam_pos = get_player_position();
		Vec3 diff_pos = vec3_difference( cam_pos, model.position );
		if ( vec3_magnitude( diff_pos ) <= 0 )
			diff_pos.z = 1;
		Vec3 billboard_rotation = get_lookat_euler_angles_rotation( vec3_normalize( diff_pos ) );
		billboard_rotation.z = get_player_rotation().z;

		a_modelspace_rotated = rotate_point_around_origin( a_modelspace_rotated, billboard_rotation ); 
		b_modelspace_rotated = rotate_point_around_origin( b_modelspace_rotated, billboard_rotation );
		c_modelspace_rotated = rotate_point_around_origin( c_modelspace_rotated, billboard_rotation ); 
	}

        a_modelspace_rotated = rotate_point_around_origin( a_modelspace_rotated, model.rotation );
        b_modelspace_rotated = rotate_point_around_origin( b_modelspace_rotated, model.rotation );
        c_modelspace_rotated = rotate_point_around_origin( c_modelspace_rotated, model.rotation );

        Vec3 a_worldspace = vec3_add(model.position, a_modelspace_rotated);
        Vec3 b_worldspace = vec3_add(model.position, b_modelspace_rotated);
        Vec3 c_worldspace = vec3_add(model.position, c_modelspace_rotated);

        Vec3 a_viewspace = worldspace_coords_to_viewspace_coords(a_worldspace);
        Vec3 b_viewspace = worldspace_coords_to_viewspace_coords(b_worldspace);
        Vec3 c_viewspace = worldspace_coords_to_viewspace_coords(c_worldspace);
        
        rasterize_and_draw_primitive_v2(a_viewspace, b_viewspace, c_viewspace, normals_ptr, UVs_ptr, model.texture, &model);
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
                float l = fabs(
			fmax(
				0.0,  
				vec3_dot_product( 
					//worldspace_coords_to_viewspace_coords( 
						directionalLight->normal, 
					//), 
					//worldspace_coords_to_viewspace_coords(
						*normal 
					//)
				) 
			)
		);

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

	RGBA fragColor = (tex != NULL && UV != NULL) ? 
		sample_texture( UV->x, UV->y, tex ) : 
		mdl->color;

	if ( fragColor.alpha == 0 ) return;

	unsigned short combined_red = (float)fragColor.red * (float)lightLevel_red / 255.0,
		combined_green = (float)fragColor.green * (float)lightLevel_green / 255.0,
		combined_blue = (float)fragColor.blue * (float)lightLevel_blue / 255.0;

	const unsigned short sqtwofivefive = pow(255, 2);
	unsigned short lightLevel = (float)( sqrt( pow( combined_red, 2 ) + pow( combined_green, 2 ) + pow( combined_blue, 2 ) ) 
		/ (float)sqrt( sqtwofivefive*3 ) ) * 255;

	set_draw_color( combined_red, combined_green, combined_blue );	
        set_frame_buffer_fragment(x, y, light_level_to_fragment(lightLevel)  );
        if ( get_depth_testing_state() == DEPTH_TESTING_STATE_ENABLED ) set_depth_buffer_depth(x, y, depth);
    }
}

void draw_ui_text( UIText *ui_txt )
{
	size_t x_screenpos = ui_txt->position.x, y_screenpos = ui_txt->position.y;
	char *text = ui_txt->text;

	RGB draw_color = ui_txt->color;

	size_t len = strlen( text );
	int x_draw_screenpos = x_screenpos, y_draw_screenpos = y_screenpos;

   	set_depth_testing_state( DEPTH_TESTING_STATE_ENABLED );

	for ( size_t i = 0; i < len; ++i )
	{
		char c = *( text + i );

		if ( c == '\n' ){
			x_draw_screenpos = x_screenpos;
			++y_draw_screenpos;
		}else{

			if ( get_ui_layers_buffer_layer( x_draw_screenpos, y_draw_screenpos ) <= ui_txt->layer )
			{
				set_color_buffer_color( x_draw_screenpos, y_draw_screenpos, draw_color );
				set_frame_buffer_fragment( x_draw_screenpos, y_draw_screenpos, c );
				set_ui_layers_buffer_layer( x_draw_screenpos, y_draw_screenpos, ui_txt->layer );
			}

			++x_draw_screenpos;
		}
	}

}

void draw_ui_frame( UIFrame *ui_frame )
{

	IntVec2 draw = ui_frame->position;
	short iterate_x = sgn( ui_frame->size.x ), iterate_y = sgn( ui_frame->size.y );
	char frag = ' ';
	float frag_depth = ui_frame->layer;

   	set_depth_testing_state( DEPTH_TESTING_STATE_ENABLED );

	for ( size_t i = 0; i < abs( ui_frame->size.y ); ++i )
	{

		for ( size_t j = 0; j < abs( ui_frame->size.x ); ++j )
		{

			if ( get_ui_layers_buffer_layer( draw.x, draw.y ) <= frag_depth )
			{

				if ( ( i == 0 || i >= abs( ui_frame->size.y ) - 1 ) && ( j == 0 || j >= abs( ui_frame->size.x ) - 1 )  ) frag = '*';
				else if ( i == 0 || i >= abs( ui_frame->size.y ) - 1 ) frag = '-';
				else if ( j == 0 || j >= abs( ui_frame->size.x ) - 1 ) frag = '|';
				else frag = ' ';
	
				set_color_buffer_color( draw.x, draw.y, ui_frame->color );
				set_frame_buffer_fragment( draw.x, draw.y, frag );
				set_ui_layers_buffer_layer( draw.x, draw.y, ui_frame->layer );

			}

			draw.x += iterate_x;
		}	
		draw.y += iterate_y;
		draw.x = ui_frame->position.x;

	}	

}

void draw_ui_image( UIImage *ui_image )
{

	IntVec2 draw = ui_image->position;
	short iterate_x = sgn( ui_image->size.x ), iterate_y = sgn( ui_image->size.y );
	char frag = ' ';
	float frag_depth = ui_image->layer;

	Vec2 draw_tex_uv = { 0, 0, };
	const Vec2 pixel_uv_coords_size = {

		1.0 / ( float ) ui_image->size.x,
		1.0 / ( float ) ui_image->size.y

	};

   	set_depth_testing_state( DEPTH_TESTING_STATE_ENABLED );

	for ( size_t i = 0; i < abs( ui_image->size.y ); ++i )
	{

		for ( size_t j = 0; j < abs( ui_image->size.x ); ++j )
		{
			RGBA draw_col = sample_texture( draw_tex_uv.x, draw_tex_uv.y, ui_image->texture );

			if ( get_ui_layers_buffer_layer( draw.x, draw.y ) <= frag_depth && draw_col.alpha > 0 )
			{
	
				set_color_buffer_color( draw.x, draw.y, RGBA_to_RGB( draw_col ) );
				set_frame_buffer_fragment( draw.x, draw.y, '@' );
				set_ui_layers_buffer_layer( draw.x, draw.y, ui_image->layer );

			}

			draw.x += iterate_x;
			draw_tex_uv.x += pixel_uv_coords_size.x;
		}	

		draw.y += iterate_y;
		draw.x = ui_image->position.x;
		draw_tex_uv.y += pixel_uv_coords_size.y;
		draw_tex_uv.x = 0;

	}	

}

void clear_console(){
        system(CLEAR_CMD);
	set_default_draw_color();
}

RGBA sample_texture(float UV_x, float UV_y, const Texture* tex)
{
	RGBA out = {255, 255, 255, 255};

	if (tex == NULL || tex->data == NULL) return out;

	size_t x = (size_t)(UV_x*(tex->width-1)) % tex->width,
		y = (size_t)(UV_y*(tex->height-1)) % tex->height;

	uint32_t pixelData = *((uint32_t*)tex->data + y*tex->width + x);
	out.alpha = pixelData >> 24 & 255;
	out.blue = pixelData >> 16 & 255;
	out.green = pixelData >> 8 & 255;
	out.red = pixelData & 255;

	return out;
}

static void draw_obj( Object* obj )
{
	switch ( obj->type ){

		case ASCIIGL_OBJTYPE_MODEL:
		case ASCIIGL_OBJTYPE_MODEL_BILLBOARD:
			draw_model( *( (Model*) obj->ptr ) );
			break;
		case ASCIIGL_OBJTYPE_UI_TEXT:
			draw_ui_text( ( UIText* ) obj->ptr );	
			break;
		case ASCIIGL_OBJTYPE_UI_FRAME:
			draw_ui_frame( ( UIFrame* ) obj->ptr );
			break;
		case ASCIIGL_OBJTYPE_UI_IMAGE:
			draw_ui_image( ( UIImage* ) obj->ptr );
			break;

	}
}

void draw()
{
	iterate_workspace( draw_obj );
}
