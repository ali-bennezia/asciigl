#include "models.h"
#include "utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int load_model_obj_strategy(const char* path, Model* destination)
{

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

	free_dynamic_array( &indices );
	free_dynamic_array( &normals );
	free_dynamic_array( &uvs );	
	free_dynamic_array( &vertices );	

	return 0;
}
