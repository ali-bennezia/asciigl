/* 

	This file is part of asciigl.

	asciigl is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

	asciigl is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with asciigl. If not, see <https://www.gnu.org/licenses/>.

*/

#include "models.h"
#include "utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int load_model_obj_strategy(const char* path, Model* destination)
{
	if (destination == NULL) return 1;
	FILE* file = fopen( path, "r" );
	if (file == NULL) return 1;

	printf("Loading model %s ...\n", path);

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

			insert_data( &normals, &ndata[3], sizeof(float)*3 );

		}else if( strcmp( token, "f" ) == 0 ){
			char* ind = strtok( NULL, " " );
			while( ind != NULL ){
				int index_a = atoi( strtok ( ind, "/" ) );
				int index_b = atoi( strtok ( NULL, "/" ) );
				int index_c = atoi( strtok ( NULL, "/" ) );

				insert_data( &indices, &index_a, sizeof(int) );
				insert_data( &indices, &index_b, sizeof(int) );
				insert_data( &indices, &index_c, sizeof(int) );

				ind = strtok( NULL, " " );
			}

		}else continue;
	}

	fclose(file);

	printf("%d vertices, %d UVs, %d normals, %d indices loaded.\n", vertices.usage, uvs.usage, normals.usage, indices.usage);

	if ( destination->mesh.buffer == NULL ) free_dynamic_array( &destination->mesh );
	if ( destination->normals.buffer == NULL ) free_dynamic_array( &destination->normals );
	if ( destination->UVs.buffer == NULL ) free_dynamic_array( &destination->UVs );
	return 0;
	if (indices.usage > 0){

		DynamicArray unwrapped_vertices = gen_dynamic_array( sizeof( float ) * 3 ),
			unwrapped_uvs = gen_dynamic_array( sizeof( float ) * 2 ),
			unwrapped_normals = gen_dynamic_array( sizeof( float ) * 3 );

		for (size_t i = 0; i < indices.usage/3; ++i){

			int index_a = *((int*)indices.buffer + i*3);
			int index_b = *((int*)indices.buffer + i*3 + 1);
			int index_c = *((int*)indices.buffer + i*3 + 2);

			//vertices unwrapping
			float uwrverts[3][3] = {{ *((float*)vertices.buffer + index_a*3), *((float*)vertices.buffer + index_a*3 + 1), *((float*)vertices.buffer + index_a*3 + 2) },
				{ *((float*)vertices.buffer + index_b*3), *((float*)vertices.buffer + index_b*3 + 1), *((float*)vertices.buffer + index_b*3 + 2) },
				{ *((float*)vertices.buffer + index_c*3), *((float*)vertices.buffer + index_c*3 + 1), *((float*)vertices.buffer + index_c*3 + 2) }
			};
			insert_data( &unwrapped_vertices, &uwrverts[0][0], sizeof(float) * 3 );
			insert_data( &unwrapped_vertices, &uwrverts[1][0], sizeof(float) * 3 );
			insert_data( &unwrapped_vertices, &uwrverts[2][0], sizeof(float) * 3 );

			//UVs unwrapping
			float uwruvs[3][2] = {{ *((float*)uvs.buffer + index_a*2), *((float*)uvs.buffer + index_a*2 + 1) },
				{ *((float*)uvs.buffer + index_b*2), *((float*)uvs.buffer + index_b*2 + 1) },
				{ *((float*)uvs.buffer + index_c*2), *((float*)uvs.buffer + index_c*2 + 1) }
			};
			insert_data( &unwrapped_uvs, &uwruvs[0][0], sizeof(float) * 2 );
			insert_data( &unwrapped_uvs, &uwruvs[1][0], sizeof(float) * 2 );
			insert_data( &unwrapped_uvs, &uwruvs[2][0], sizeof(float) * 2 );

			//normals unwrapping
			float uwrnormals[3][3] = {{ *((float*)normals.buffer + index_a*3), *((float*)normals.buffer + index_a*3 + 1), *((float*)normals.buffer + index_a*3 + 2) },
				{ *((float*)normals.buffer + index_b*3), *((float*)normals.buffer + index_b*3 + 1), *((float*)normals.buffer + index_b*3 + 2) },
				{ *((float*)normals.buffer + index_c*3), *((float*)normals.buffer + index_c*3 + 1), *((float*)normals.buffer + index_c*3 + 2) }
			};
			insert_data( &unwrapped_normals, &uwrnormals[0][0], sizeof(float) * 3 );
			insert_data( &unwrapped_normals, &uwrnormals[1][0], sizeof(float) * 3 );
			insert_data( &unwrapped_normals, &uwrnormals[2][0], sizeof(float) * 3 );
				
		}

		free_dynamic_array( &indices );
		free_dynamic_array( &normals );
		free_dynamic_array( &uvs );	
		free_dynamic_array( &vertices );

		destination->mesh = unwrapped_vertices;
		destination->UVs = unwrapped_uvs;
		destination->normals = unwrapped_normals;	

	}else{
		destination->mesh = vertices;
		destination->UVs = uvs;
		destination->normals = normals;
	}

	return 0;
}
