#include "models.h"
#include "utils.h"

#include <stdlib.h>
#include <stdio.h>

int load_model_obj_strategy(const char* path, Model* destination)
{

	FILE* file = fopen( path, "r" );
	if (file == NULL) return 1;

	char line[1024];

	while (fgets(line, sizeof(line), file))
	{
		printf("%s", line);	
	}	

	fclose(file);	

	return 0;
}
