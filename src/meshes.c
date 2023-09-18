/* 

	This file is part of asciigl.

	asciigl is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

	asciigl is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with asciigl. If not, see <https://www.gnu.org/licenses/>.

*/

#include "./../include/meshes.h"
#include "./../include/utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Mesh *load_mesh_obj_strategy(const char* path)
{
	FILE* file = fopen( path, "r" );
	if (file == NULL) return NULL;

	int detectedPrimitive = 0;
	enum PRIMITIVE_TYPE primitiveType = TRIANGLE_PRIMITIVE;

	DynamicArray vertices = gen_dynamic_array( sizeof( float ) * 3 ),
		uvs = gen_dynamic_array( sizeof( float ) * 2 ),
		normals = gen_dynamic_array( sizeof( float ) * 3 ),
		indices = gen_dynamic_array( sizeof( int ) );

	char line[1024];
	while (fgets(line, sizeof(line), file))
	{
		const char* token = strtok( line, " " );

		if ( strcmp( token, "v" ) == 0 ){

			float vertex_x = atof( strtok( NULL, " " ) );
			float vertex_y = atof( strtok( NULL, " " ) );
			float vertex_z = atof( strtok( NULL, " " ) );
			float vdata[3] = { vertex_x, vertex_y, vertex_z };

			insert_data( &vertices, &vdata[0], sizeof(float)*3 );

		}else if( strcmp( token, "vt" ) == 0 ){

			float uv_x = atof( strtok( NULL, " " ) );
			float uv_y = atof( strtok( NULL, " " ) );
			float uvdata[2] = { uv_x, uv_y };

			insert_data( &uvs, &uvdata[0], sizeof(float)*2 );

		}else if( strcmp( token, "vn" ) == 0 ){

			float normal_x = atof( strtok( NULL, " " ) );
			float normal_y = atof( strtok( NULL, " " ) );
			float normal_z = atof( strtok( NULL, " " ) );
			float ndata[3] = { normal_x, normal_y, normal_z };

			insert_data( &normals, &ndata[0], sizeof(float)*3 );

		}else if( strcmp( token, "f" ) == 0 ){
			DynamicArray tokens = gen_dynamic_array( sizeof(char*) );

			char* cur_token = strtok( NULL, " " );
			while ( cur_token != NULL ){
				insert_data( &tokens, &cur_token, sizeof(char*) );
				cur_token = strtok( NULL, " " );
			}

			for (size_t i = 0; i < tokens.usage; ++i){
				char* ind = *((char**)tokens.buffer + i);

				// printf("Processing reference token: %s\n", ind);
				int index_a = atoi( strtok ( ind, "/" ) );
				int index_b = atoi( strtok ( NULL, "/" ) );
				int index_c = atoi( strtok ( NULL, "/" ) );

				insert_data( &indices, &index_a, sizeof(int) );
				insert_data( &indices, &index_b, sizeof(int) );
				insert_data( &indices, &index_c, sizeof(int) );
			}

			if (!detectedPrimitive){
				detectedPrimitive = 1;
				if ( floatmod( tokens.usage, 3 ) == 0 )
					primitiveType = TRIANGLE_PRIMITIVE;
				else if ( floatmod( tokens.usage, 4 ) == 0 )
					primitiveType = QUAD_PRIMITIVE;
			}

			free_dynamic_array( &tokens );
		}else continue;
	}

	fclose(file);

	Mesh *destination = gen_mesh();
	free_dynamic_array( &destination->vertices );
	free_dynamic_array( &destination->UVs );
	free_dynamic_array( &destination->normals );

	if (indices.usage > 0){

		DynamicArray unwrapped_vertices = gen_dynamic_array( sizeof( float ) * 3 ),
			unwrapped_uvs = gen_dynamic_array( sizeof( float ) * 2 ),
			unwrapped_normals = gen_dynamic_array( sizeof( float ) * 3 );

		for (size_t i = 0; i < indices.usage/3; ++i){
			
			int index_a = *((int*)indices.buffer + i*3) - 1;
			int index_b = *((int*)indices.buffer + i*3 + 1) - 1;
			int index_c = *((int*)indices.buffer + i*3 + 2) - 1;

			//vertices unwrapping
			insert_data( &unwrapped_vertices, (float*)vertices.buffer + index_a*3, sizeof(float) * 3 );

			//UVs unwrapping
			insert_data( &unwrapped_uvs, (float*)uvs.buffer + index_b*2, sizeof(float) * 2 );

			//normals unwrapping
			insert_data( &unwrapped_normals, (float*)normals.buffer + index_c*3, sizeof(float) * 3 );
				
		}

		free_dynamic_array( &normals );
		free_dynamic_array( &uvs );	
		free_dynamic_array( &vertices );

		destination->vertices = unwrapped_vertices;
		destination->UVs = unwrapped_uvs;
		destination->normals = unwrapped_normals;

	}else{
		destination->vertices = vertices;
		destination->UVs = uvs;
		destination->normals = normals;
	}

	if ( primitiveType == QUAD_PRIMITIVE ){
		DynamicArray tri_vertices = gen_dynamic_array( sizeof(float)*3 ),
			tri_UVs = gen_dynamic_array( sizeof(float)*2 ),
			tri_normals = gen_dynamic_array( sizeof(float)*3 );

		for( size_t i = 0; i < destination->vertices.usage / 4; ++i ){
			float vert_a[3] = {
				*((float*)destination->vertices.buffer + i * 12),
				*((float*)destination->vertices.buffer + i * 12 + 1),
				*((float*)destination->vertices.buffer + i * 12 + 2)
			};
			float vert_b[3] = {
				*((float*)destination->vertices.buffer + i * 12 + 3),
				*((float*)destination->vertices.buffer + i * 12 + 4),
				*((float*)destination->vertices.buffer + i * 12 + 5)
			};
			float vert_c[3] = {
				*((float*)destination->vertices.buffer + i * 12 + 6),
				*((float*)destination->vertices.buffer + i * 12 + 7),
				*((float*)destination->vertices.buffer + i * 12 + 8)
			};		
			float vert_d[3] = {
				*((float*)destination->vertices.buffer + i * 12 + 9),
				*((float*)destination->vertices.buffer + i * 12 + 10),
				*((float*)destination->vertices.buffer + i * 12 + 11)
			};
			
			insert_data( &tri_vertices, &vert_a[0], sizeof(float)*3 );	
			insert_data( &tri_vertices, &vert_b[0], sizeof(float)*3 );
			insert_data( &tri_vertices, &vert_c[0], sizeof(float)*3 );	
	
			insert_data( &tri_vertices, &vert_c[0], sizeof(float)*3 );	
			insert_data( &tri_vertices, &vert_d[0], sizeof(float)*3 );	
			insert_data( &tri_vertices, &vert_a[0], sizeof(float)*3 );

			if ( destination->UVs.usage == destination->vertices.usage ){

				float UVs_a[3] = {
					*((float*)destination->UVs.buffer + i * 8),
					*((float*)destination->UVs.buffer + i * 8 + 1)
				};
				float UVs_b[3] = {
					*((float*)destination->UVs.buffer + i * 8 + 2),
					*((float*)destination->UVs.buffer + i * 8 + 3)
				};
				float UVs_c[3] = {
					*((float*)destination->UVs.buffer + i * 8 + 4),
					*((float*)destination->UVs.buffer + i * 8 + 5)
				};		
				float UVs_d[3] = {
					*((float*)destination->UVs.buffer + i * 8 + 6),
					*((float*)destination->UVs.buffer + i * 8 + 7)
				};

				insert_data( &tri_UVs, &UVs_a[0], sizeof(float)*2 );	
				insert_data( &tri_UVs, &UVs_b[0], sizeof(float)*2 );
				insert_data( &tri_UVs, &UVs_c[0], sizeof(float)*2 );	
		
				insert_data( &tri_UVs, &UVs_c[0], sizeof(float)*2 );	
				insert_data( &tri_UVs, &UVs_d[0], sizeof(float)*2 );	
				insert_data( &tri_UVs, &UVs_a[0], sizeof(float)*2 );

			}
			
			if ( destination->normals.usage == destination->vertices.usage ){

				float normals_a[3] = {
					*((float*)destination->normals.buffer + i * 12),
					*((float*)destination->normals.buffer + i * 12 + 1),
					*((float*)destination->normals.buffer + i * 12 + 2)
				};
				float normals_b[3] = {
					*((float*)destination->normals.buffer + i * 12 + 3),
					*((float*)destination->normals.buffer + i * 12 + 4),
					*((float*)destination->normals.buffer + i * 12 + 5)
				};
				float normals_c[3] = {
					*((float*)destination->normals.buffer + i * 12 + 6),
					*((float*)destination->normals.buffer + i * 12 + 7),
					*((float*)destination->normals.buffer + i * 12 + 8)
				};		
				float normals_d[3] = {
					*((float*)destination->normals.buffer + i * 12 + 9),
					*((float*)destination->normals.buffer + i * 12 + 10),
					*((float*)destination->normals.buffer + i * 12 + 11)
				};
				
				insert_data( &tri_normals, &normals_a[0], sizeof(float)*3 );	
				insert_data( &tri_normals, &normals_b[0], sizeof(float)*3 );
				insert_data( &tri_normals, &normals_c[0], sizeof(float)*3 );	
		
				insert_data( &tri_normals, &normals_c[0], sizeof(float)*3 );	
				insert_data( &tri_normals, &normals_d[0], sizeof(float)*3 );	
				insert_data( &tri_normals, &normals_a[0], sizeof(float)*3 );

			}

		}

		free_dynamic_array( &destination->vertices );
		free_dynamic_array( &destination->UVs );
		free_dynamic_array( &destination->normals );

		destination->vertices = tri_vertices;
		destination->UVs = tri_UVs;
		destination->normals = tri_normals;
	}

	destination->vertices.usage /= 3;

	free_dynamic_array( &indices );

	return destination;
}

Mesh *gen_mesh()
{
	Mesh *mesh = malloc( sizeof( Mesh ) );

	mesh->vertices = gen_dynamic_array( sizeof(Triangle) );
	mesh->normals = gen_dynamic_array( sizeof(Vec3) );
	mesh->UVs = gen_dynamic_array( sizeof(Vec2) );

	return mesh;
}

void free_mesh( Mesh *mesh )
{
	if ( !mesh ) return;
	free_dynamic_array( &mesh->vertices );
	free_dynamic_array( &mesh->UVs );
	free_dynamic_array( &mesh->normals );
	free( mesh );
}

Mesh *load_mesh(const char* path)
{
	char *extension = get_path_extension_alloc( ( char* ) path );
	Mesh *result;
	if ( strcmp( extension, "obj" ) == 0 )
	{
		result = load_mesh_obj_strategy( path );
	}else{
		fprintf(stderr, "Unsupported model file type. Extension: %s\n", extension);
		result = NULL;
	}
	free( extension );
	return result;
}
