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

#include <assert.h>

static int get_ln_tokens( char *ln, char **tokens, size_t *amnt )
{
	size_t index = 0;
	char *tk = NULL;
	while ( ( tk = strtok( index == 0 ? ln : NULL, " " ) ) && index < 256 )
	{
		*( tokens + index ) = tk;	
		++index;	
	}
	*amnt = index;
	return index < 0 || index > 256;	
}

typedef struct Indices {
	int a, b, c;
} Indices;

static Indices get_str_indices( char *str )
{
	Indices out = {0};

	size_t i = 0;
	char *tok = NULL;
	while ( ( tok = strtok( i == 0 ? str : NULL, "/" ) ) && i < 3 )
	{
		*( ( int* ) &out + i ) = atoi( tok );
		++i;
	}	

	return out;
}

static void convert_indices_quads_to_tris( DynamicArray *quad_indices, DynamicArray *tri_indices )
{

	for ( size_t i = 0; i < quad_indices->usage / 4; ++i )
	{
		Indices vert_1 = *(( Indices* ) quad_indices->buffer + i * 4 + 0 ),
			vert_2 = *(( Indices* ) quad_indices->buffer + i * 4 + 1 ),
			vert_3 = *(( Indices* ) quad_indices->buffer + i * 4 + 2 ),
			vert_4 = *(( Indices* ) quad_indices->buffer + i * 4 + 3 );

		insert_data( tri_indices, &vert_1, sizeof( int ) * 3 );
		insert_data( tri_indices, &vert_3, sizeof( int ) * 3 );
		insert_data( tri_indices, &vert_2, sizeof( int ) * 3 );

		insert_data( tri_indices, &vert_1, sizeof( int ) * 3 );
		insert_data( tri_indices, &vert_4, sizeof( int ) * 3 );
		insert_data( tri_indices, &vert_3, sizeof( int ) * 3 );
	}

}

static void unpack_indices_data( 
	DynamicArray *vertices, 
	DynamicArray *UVs, 
	DynamicArray *normals, 
	DynamicArray *indices, 
	DynamicArray *out_vertices, 
	DynamicArray *out_UVs, 
	DynamicArray *out_normals 
)
{
	for ( size_t i = 0; i < indices->usage; ++i )
	{
		Indices ind = *( ( Indices* ) indices->buffer + i );
		--ind.a; --ind.b; --ind.c;

		Vec3 vertex = *( ( Vec3* ) vertices->buffer + ind.a );
		Vec2 UVcoords = *( ( Vec2* ) UVs->buffer + ind.b );	
		Vec3 normal = *( ( Vec3* ) normals->buffer + ind.c );

		insert_data( out_vertices, &vertex, sizeof( float ) * 3 );	
		insert_data( out_UVs, &UVcoords, sizeof( float ) * 2 );	
		insert_data( out_normals, &normal, sizeof( float ) * 3 );
	}
}

Mesh *load_mesh_obj_strategy(const char* path)
{
	FILE* file = fopen( path, "r" );
	if (file == NULL) return NULL;

	Mesh *mesh = gen_mesh();

	DynamicArray raw_vertices = gen_dynamic_array( sizeof( float ) * 3 ), 
		raw_UVs = gen_dynamic_array( sizeof( float ) * 2 ), 
		raw_normals = gen_dynamic_array( sizeof( float ) * 3 ),
		tri_indices = gen_dynamic_array( sizeof( int ) * 3 ),
		quad_indices = gen_dynamic_array( sizeof( int ) * 3 );

	char line[256];
	while ( fgets( &line[0], 256, file ) )
	{

		char *line_tokens[ 256 ];
		size_t line_tokens_amnt = 0;
		get_ln_tokens( &line[0], &line_tokens[0], &line_tokens_amnt );

		if ( line_tokens_amnt == 0 ) continue;

		if ( strcmp( line_tokens[0], "#" ) == 0 ){
			continue;
		}else if ( strcmp( line_tokens[0], "v" ) == 0 ){
			float x = atof( convert_float_string_to_current_locale( line_tokens[1] ) ),
				y = atof( convert_float_string_to_current_locale( line_tokens[2] ) ),
				z = atof( convert_float_string_to_current_locale( line_tokens[3] ) );
			
			Vec3 vertex = { x, y, z };
			insert_data( &raw_vertices, &vertex, sizeof( float ) * 3 );
		}else if ( strcmp( line_tokens[0], "vt" ) == 0 ){
			float x = atof( convert_float_string_to_current_locale( line_tokens[1] ) ),
				y = atof( convert_float_string_to_current_locale( line_tokens[2] ) );
			
			Vec2 UV = { x, y };
			insert_data( &raw_UVs, &UV, sizeof( float ) * 2 );
		}else if ( strcmp( line_tokens[0], "vn" ) == 0 ){
			float x = atof( convert_float_string_to_current_locale( line_tokens[1] ) ),
				y = atof( convert_float_string_to_current_locale( line_tokens[2] ) ),
				z = atof( convert_float_string_to_current_locale( line_tokens[3] ) );
			
			Vec3 normal = { x, y, z };
			insert_data( &raw_normals, &normal, sizeof( float ) * 3 );
		}else if ( strcmp( line_tokens[0], "f" ) == 0 ){
			DynamicArray *indice_dest = ( ( line_tokens_amnt - 1 ) % 4 == 0 ) ? &quad_indices : &tri_indices;

			for ( size_t i = 0; i < line_tokens_amnt - 1; ++i )
			{
				Indices indices = get_str_indices( line_tokens[ 1 + i ] );
				insert_data( indice_dest, &indices, sizeof( int ) * 3 );
			}
		}

	} 

	convert_indices_quads_to_tris( &quad_indices, &tri_indices );
	unpack_indices_data( &raw_vertices, &raw_UVs, &raw_normals, &tri_indices, &mesh->vertices, &mesh->UVs, &mesh->normals );

	free_dynamic_array( &raw_vertices );
	free_dynamic_array( &raw_UVs );
	free_dynamic_array( &raw_normals );
	free_dynamic_array( &tri_indices );
	free_dynamic_array( &quad_indices );

	return mesh;
}

Mesh *gen_mesh()
{
	Mesh *mesh = malloc( sizeof( Mesh ) );

	mesh->vertices = gen_dynamic_array( sizeof(Vec3) );
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
		result = load_mesh_obj_strategy( ( char* ) path );
	}else{
		fprintf(stderr, "Unsupported model file type. Extension: %s\n", extension);
		result = NULL;
	}
	free( extension );
	return result;
}
