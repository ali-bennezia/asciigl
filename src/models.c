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

	DynamicArray vertices = gen_dynamic_array( sizeof( float ) * 3 );

	char line[1024];
	while (fgets(line, sizeof(line), file))
	{
		const char* token = strtok( line, " " );

		if ( strcmp( token, "v" ) == 0 ){

			float vertex_x = atof( strtok( NULL, " " ) );
			float vertex_y = atof( strtok( NULL, " " ) );
			float vertex_z = atof( strtok( NULL, " " ) );

			printf("x %f - y %f - z %f\n", vertex_x, vertex_y, vertex_z);

		}else if( strcmp( token, "vt" ) == 0 ){

		}else if( strcmp( token, "vn" ) == 0 ){

		}else if( strcmp( token, "f" ) ){

		}else continue;
	}	

	fclose(file);




	return 0;
}
