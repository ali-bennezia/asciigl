#include "utils.h"
#include "state.h"

#include <string.h>

//utils

float floatmod(float a, float b){
    return a - ((int)(a/b))*b;
}

//dataSize: data type size in bytes
DynamicArray gen_dynamic_array(size_t dataSize){
    DynamicArray out;
    out.buffer = malloc( dataSize*10 );
    out.size = 10;
    out.usage = 0;
    return out;
}

void insert_data(DynamicArray* arr, void* data, size_t dataSize)
{
    if (arr->usage == arr->size){
        arr->size *= 2;
        arr->buffer = realloc(arr->buffer, arr->size * dataSize);
    }

    memcpy( ((char*)arr->buffer) + arr->usage*dataSize, data, dataSize);
    ++arr->usage;
}

void set_data(DynamicArray* arr, size_t index, void* data, size_t dataSize){
    memcpy( ((char*)arr->buffer + index*dataSize), data, dataSize );
}

void* get_data(DynamicArray* arr, size_t index, size_t dataSize){
    return (void*)(((char*)arr->buffer) + index*dataSize);
}

void remove_data(DynamicArray* arr, size_t index, size_t dataSize)
{
    if (index < 0 || index >= arr->usage)
        return;

    if (index < arr->usage-1)
        memcpy( ((char*)arr->buffer) + index*dataSize, ((char*)arr->buffer) + (index+1)*dataSize, (arr->usage-(index+1))*dataSize );

    --arr->usage;
}

void free_dynamic_array(DynamicArray* arr){
    free( arr->buffer );
    arr->usage = 0;
    arr->size = 0;
}

float to_rads(float degrees){
    return degrees / 180.0 * M_PI;
}

float to_degs(float rads){
    return rads / M_PI * 180.0;
}

float vec2_magnitude(Vec2 vec){
    return sqrt( vec.x * vec.x + vec.y * vec.y );
}

float vec2_dot_product(Vec2 a, Vec2 b){
    return a.x * b.x + a.y * b.y;
}

Vec2 vec2_normalize(Vec2 vec){
    Vec2 out;
    float magnitude = vec2_magnitude(vec);
    if (magnitude == 0)
    {
        out.x = 0;
        out.y = 0;
    }else{
        out.x = vec.x /= magnitude;
        out.y = vec.y /= magnitude;
    }
    return out;
}

Vec2 vec2_mirror(Vec2 vec){
    Vec2 out;
    out.x = vec.x * -1.0;
    out.y = vec.y * -1.0;
    return out;
}

Vec2 vec2_add(Vec2 a, Vec2 b){
    Vec2 out;
    out.x = a.x + b.x;
    out.y = a.y + b.y;
    return out;
}

Vec2 vec2_multiplication(Vec2 vec, float scalar){
    Vec2 out;
    out.x = vec.x * scalar;
    out.y = vec.y * scalar;
    return out;
}

float vec2_division(Vec2 dividand, Vec2 divider){
    Vec2 normalized_dividand = vec2_normalize(dividand), normalized_divider = vec2_normalize(divider);
    float parallelity = vec2_dot_product(normalized_dividand, normalized_divider);
    float divider_magnitude = vec2_magnitude(divider);
    if (parallelity == 0 || divider_magnitude == 0)
        return 0;

    return vec2_magnitude(dividand) * parallelity / divider_magnitude;
}

Vec2 vec2x2_average(Vec2 a, Vec2 b){
    Vec2 out;
    out.x = (a.x + b.x) / 2.0;
    out.y = (a.y + b.y) / 2.0;
    return out;
}

Vec2 vec2x3_average(Vec2 a, Vec2 b, Vec2 c){
    Vec2 out;
    out.x = (a.x + b.x + c.x) / 3.0;
    out.y = (a.y + b.y + c.y) / 3.0;
    return out;
}

Vec2 vec2_difference(Vec2 a, Vec2 b){
    Vec2 out;
    out.x = a.x - b.x;
    out.y = a.y - b.y;
    return out;
}

float vec3_magnitude(Vec3 vec){
    return sqrt( vec.x * vec.x + vec.y * vec.y + vec.z * vec.z );
}

float vec3_dot_product(Vec3 a, Vec3 b){
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

Vec3 vec3_cross_product(Vec3 a, Vec3 b){
    Vec3 out;
    out.z = a.x*b.y - a.y*b.x;
    out.x = a.y*b.z - a.z*b.y;
    out.y = a.z*b.x - a.x*b.z;
    return out;
}

Vec3 vec3_normalize(Vec3 vec){
    float magnitude = vec3_magnitude(vec);
    if (magnitude == 0)
    {
        vec.x = 0;
        vec.y = 0;
        vec.z = 0;
        return vec;
    }else{
        vec.x /= magnitude;
        vec.y /= magnitude;
        vec.z /= magnitude;
        return vec;
    }
}

float vec3_division(Vec3 dividand, Vec3 divider){
    Vec3 normalized_dividand = vec3_normalize(dividand), normalized_divider = vec3_normalize(divider);
    float parallelity = vec3_dot_product(normalized_dividand, normalized_divider);
    float divider_magnitude = vec3_magnitude(divider);
    if (parallelity == 0 || divider_magnitude == 0)
        return 0;

    return vec3_magnitude(dividand) * parallelity / divider_magnitude;
}

Vec3 vec3_multiplication(Vec3 vec, float scalar)
{
    Vec3 out;
    out.x = vec.x * scalar;
    out.y = vec.y * scalar;
    out.z = vec.z * scalar;
    return out;
}

Vec3 vec3_mirror(Vec3 vec){
    Vec3 out;
    out.x = -vec.x;
    out.y = -vec.y;
    out.z = -vec.z;
    return out;
}

Vec3 vec3_add(Vec3 a, Vec3 b){
    Vec3 out;
    out.x = a.x + b.x;
    out.y = a.y + b.y;
    out.z = a.z + b.z;
    return out;
}

Vec3 vec3_difference(Vec3 a, Vec3 b){
    Vec3 out;
    out.x = a.x - b.x;
    out.y = a.y - b.y;
    out.z = a.z - b.z;
    return out;
}

Vec3 vec3_scale(Vec3 vec, Vec3 scale){
    vec.x *= scale.x;
    vec.y *= scale.y;
    vec.z *= scale.z;
    return vec;
}

Vec3 rotate_point_around_origin(Vec3 position, Vec3 rotation){

    float x_rads = to_rads(rotation.x), y_rads = to_rads(rotation.y), z_rads = to_rads(rotation.z);

    float rotation_x_sin = sin( x_rads );
    float rotation_x_cos = cos( x_rads );

    float rotation_y_sin = sin( y_rads );
    float rotation_y_cos = cos( y_rads );

    float rotation_z_sin = sin( z_rads );
    float rotation_z_cos = cos( z_rads );

    Vec3 out;

    float zy_plane_distance = sqrt(position.z * position.z + position.y * position.y);

    if (zy_plane_distance != 0){

        float zy_plane_initial_sin = position.y / zy_plane_distance;
        float zy_plane_initial_cos = position.z / zy_plane_distance;

        float zy_plane_new_sin = zy_plane_initial_sin * rotation_x_cos + rotation_x_sin * zy_plane_initial_cos;
        float zy_plane_new_cos = zy_plane_initial_cos * rotation_x_cos - rotation_x_sin * zy_plane_initial_sin;

        out.z = zy_plane_new_cos * zy_plane_distance;
        out.y = zy_plane_new_sin * zy_plane_distance;

    }else{
        out.z = 0;
        out.y = 0;
    }

    float xz_plane_distance = sqrt(position.x * position.x + out.z * out.z);

    if (xz_plane_distance != 0){
        float xz_plane_initial_sin = out.z / xz_plane_distance;
        float xz_plane_initial_cos = position.x / xz_plane_distance;

        float xz_plane_new_sin = xz_plane_initial_sin * rotation_y_cos + xz_plane_initial_cos * rotation_y_sin;
        float xz_plane_new_cos = xz_plane_initial_cos * rotation_y_cos - xz_plane_initial_sin * rotation_y_sin;

        out.x = xz_plane_new_cos * xz_plane_distance;
        out.z = xz_plane_new_sin * xz_plane_distance;
    }else{
        out.x = 0;
        out.z = 0;
    }

    float xy_plane_distance = sqrt(out.x * out.x + out.y * out.y);

    if (xy_plane_distance != 0){

        float xy_plane_initial_sin = out.y / xy_plane_distance;
        float xy_plane_initial_cos = out.x / xy_plane_distance;

        float xy_plane_new_sin = xy_plane_initial_sin * rotation_z_cos + xy_plane_initial_cos * rotation_z_sin;
        float xy_plane_new_cos = xy_plane_initial_cos * rotation_z_cos - xy_plane_initial_sin * rotation_z_sin;

        out.y = xy_plane_new_sin * xy_plane_distance;
        out.x = xy_plane_new_cos * xy_plane_distance;
    }else{
        out.y = 0;
        out.x = 0;
    }



    return out;

}

TriangularCoordinates calculate_triangular_coordinates(Vec2 a, Vec2 b, Vec2 c, Vec2 p){
    TriangularCoordinates out;

    out.a = a;
    out.b = b;
    out.c = c;

    float ab = vec2_magnitude( vec2_difference(b, a) );
    float bc = vec2_magnitude( vec2_difference(c, b) );
    float ac = vec2_magnitude( vec2_difference(c, a) );

    float pb = vec2_magnitude( vec2_difference(p, b) );
    float pc = vec2_magnitude( vec2_difference(p, c) );

    float beta = bc == 0 || ab == 0 ? 0 : acos( (ac*ac-bc*bc-ab*ab)/(-2.0*bc*ab) );
    float gamma_prime = pc == 0 || bc == 0 ? 0 : acos( (pb*pb-pc*pc-bc*bc)/(-2.0*pc*bc) );
    float alpha_prime = M_PI - beta - gamma_prime;

    float alpha_prime_sin = sin(alpha_prime);

    float double_crad_ibc = alpha_prime_sin == 0 ? 0 : bc/alpha_prime_sin;
    float ib = double_crad_ibc*sin(gamma_prime);
    float ic = double_crad_ibc*sin(beta);

    float ab_weight = ic == 0 ? 0 : pc/ic;

    out.a_weight =  ab == 0 ? 0 : ib/ab;
    out.b_weight = 1.0 - out.a_weight;
    out.c_weight = 1.0 - ab_weight;

    out.a_weight *= ab_weight;
    out.b_weight *= ab_weight;
    
    return out;
}

Vec2 vec2_int_to_float(IntVec2 vec)
{
    Vec2 out;
    out.x = (float)vec.x;
    out.y = (float)vec.y;
    return out;
}

IntVec2 vec2_float_to_int(IntVec2 vec)
{
    IntVec2 out;
    out.x = (int)vec.x;
    out.y = (int)vec.y;
    return out;
}


void insert_primitive(Model* mdl, Triangle primitive){
    insert_data( &mdl->mesh, &primitive, sizeof(Triangle) );
}

//primitivesCount is the number of Triangles ie the amount of elements in the array 'primitives' divided by 9
void insert_primitives(Model* mdl, float* primitives, size_t primitivesCount)
{
    for (size_t i = 0; i < primitivesCount; ++i){
        Triangle primitive;
        primitive.a.x = primitives[i*9];
        primitive.a.y = primitives[i*9 + 1];
        primitive.a.z = primitives[i*9 + 2];
        
        primitive.b.x = primitives[i*9 + 3];
        primitive.b.y = primitives[i*9 + 4];
        primitive.b.z = primitives[i*9 + 5];

        primitive.c.x = primitives[i*9 + 6];
        primitive.c.y = primitives[i*9 + 7];
        primitive.c.z = primitives[i*9 + 8];

        insert_primitive(mdl, primitive);
    }

}

void insert_primitive_normals(Model* mdl, Vec3 normals[3]){
    insert_data( &mdl->normals, &normals[0], sizeof(Vec3) );
    insert_data( &mdl->normals, &normals[1], sizeof(Vec3) );
    insert_data( &mdl->normals, &normals[2], sizeof(Vec3) );
}

void insert_primitives_normals(Model* mdl, float* normals, size_t primitivesCount)
{
    for (size_t i = 0; i < primitivesCount; ++i){
        Vec3 out_normals[3];

        Vec3 normal1, normal2, normal3;

        normal1.x = normals[i*9];
        normal1.y = normals[i*9 + 1];
        normal1.z = normals[i*9 + 2];

        normal2.x = normals[i*9 + 3];
        normal2.y = normals[i*9 + 4];
        normal2.z = normals[i*9 + 5];

        normal3.x = normals[i*9 + 6];
        normal3.y = normals[i*9 + 7];
        normal3.z = normals[i*9 + 8];

        out_normals[0] = normal1; out_normals[1] = normal2; out_normals[3] = normal3;

        insert_primitive_normals(mdl, out_normals);
    }
}

Model gen_model(){
    Model mdl;
    
    Vec3 pos, rot, scale;
    pos.x = 0; pos.y = 0; pos.z = 0;
    rot.x = 0; rot.y = 0; rot.z = 0;
    scale.x = 1; scale.y = 1; scale.z = 1;

    mdl.position = pos;
    mdl.rotation = rot;
    mdl.scale = scale;

    mdl.mesh = gen_dynamic_array( sizeof(Triangle) );
    mdl.normals = gen_dynamic_array( sizeof(Vec3)*3 );

    return mdl;
}

void free_model(Model mdl){
    free_dynamic_array(&mdl.mesh);
    free_dynamic_array(&mdl.normals);
}

size_t get_dword_mask_offset( uint32_t* dword )
{
	size_t val = *dword, offset = 0, iteration = val;

	if (val == 0) return 0;

	while (iteration % 2 == 0){
		iteration /= 2;
		++offset;
	}

	return offset;
}

void bmp_decode_rle_8bit_strategy( void* data, void* destination, void* palette, int32_t width, int32_t height, size_t dataSizeInBytes, enum BMP_INFO_HEADER_TYPE headerType )
{
	uint32_t mask = headerType == BMP_HEADER_V5 ? 0x00FFFFFF : 0xFFFFFFFF;

	const size_t max = dataSizeInBytes / 2;
	size_t x = 0, y = height - 1, pixelCount = y*width + x;
	for (size_t i = 0; i < max; ++i){

		uint16_t dataWord = *( (uint16_t*)data + i );
		uint8_t firstByte = dataWord >> 8 & 255, secondByte = dataWord & 255;
		
		if (firstByte > 0){
			uint32_t pixelData = *( (uint32_t*)palette + secondByte ) & mask;

			for (size_t p = 0; p < firstByte; ++p){
				pixelCount = y*width + x;
				*((uint32_t*)data + pixelCount) = pixelData;
				++x;
				if (x == width)
				{
					x = 0;
					--y;
				}
			}
	
		}else if (firstByte == 0 && secondByte <= 0x02){
			switch (secondByte){
				case 0x00:
					x = 0;
					--y;
					pixelCount = y*width + x;
					break;
				case 0x01:
					i = max;
					break;					
				default:
					uint16_t nextWord = *( (uint16_t*)data + i + 1 );
					int8_t h = nextWord >> 8, v = nextWord;
					++i;
					x += h;
					y -= v;
					pixelCount = y*width + x;
					break;
			}
		}else{
			
		}

	} 
}

void bmp_raw_data_unpack_1bit_strategy( uint8_t* byte, void* palette, uint32_t* pixelDataTarget, enum BMP_INFO_HEADER_TYPE headerType, size_t* pixelCounter ){

	uint8_t firstIndex = *byte >> 7 & 1;
	uint8_t secondIndex = *byte >> 6 & 1;
	uint8_t thirdIndex = *byte >> 5 & 1;
	uint8_t fourthIndex = *byte >> 4 & 1;
	uint8_t fifthIndex = *byte >> 3 & 1;
	uint8_t sixthIndex = *byte >> 2 & 1;
	uint8_t seventhIndex = *byte >> 1 & 1;
	uint8_t eighthIndex = *byte & 1 ;

	uint32_t mask = headerType == BMP_HEADER_V5 ? 0x00FFFFFF : 0xFFFFFFFF;

	*pixelDataTarget = *( (uint32_t*)palette + firstIndex ) & mask;
	*(pixelDataTarget + 1) = *( (uint32_t*)palette + secondIndex ) & mask;
	*(pixelDataTarget + 2) = *( (uint32_t*)palette + thirdIndex ) & mask;
	*(pixelDataTarget + 3) = *( (uint32_t*)palette + fourthIndex ) & mask;
	*(pixelDataTarget + 4) = *( (uint32_t*)palette + fifthIndex ) & mask;
	*(pixelDataTarget + 5) = *( (uint32_t*)palette + sixthIndex ) & mask;
	*(pixelDataTarget + 6) = *( (uint32_t*)palette + seventhIndex ) & mask;
	*(pixelDataTarget + 7) = *( (uint32_t*)palette + eighthIndex ) & mask;

	*pixelCounter += 8;

}

void bmp_raw_data_unpack_4bits_strategy( uint8_t* byte, void* palette, uint32_t* pixelDataTarget, enum BMP_INFO_HEADER_TYPE headerType, size_t* pixelCounter ){
	
	uint8_t firstIndex = *byte >> 4 & 15;
	uint8_t secondIndex = *byte & 15; 

	uint32_t mask = headerType == BMP_HEADER_V5 ? 0x00FFFFFF : 0xFFFFFFFF;
	
	*pixelDataTarget = *( (uint32_t*)palette  + firstIndex ) & mask;
	*(pixelDataTarget + 1) = *( (uint32_t*)palette  + secondIndex ) & mask;

	*pixelCounter += 2;

}

void bmp_raw_data_unpack_8bits_strategy( uint8_t* byte, void* palette, uint32_t* pixelDataTarget, enum BMP_INFO_HEADER_TYPE headerType, size_t* pixelCounter ){

	uint8_t index = *byte;

	uint32_t mask = headerType == BMP_HEADER_V5 ? 0x00FFFFFF : 0xFFFFFFFF;

	*pixelDataTarget = *( (uint32_t*)palette + index ) & mask;

	++(*pixelCounter);

}

void bmp_raw_data_unpack_24bits_strategy( uint8_t* byte, void* palette, uint32_t* pixelDataTarget, enum BMP_INFO_HEADER_TYPE headerType, size_t* pixelCounter ){

	size_t blue = *byte, green = *( byte + 1 ), red = *( byte + 2 );
	*((uint8_t*)pixelDataTarget) = red;
	*((uint8_t*)pixelDataTarget + 1) = green;
	*((uint8_t*)pixelDataTarget + 2) = blue;
	*((uint8_t*)pixelDataTarget + 3) = 255;

	++(*pixelCounter);

}

void bmp_raw_data_unpack_32bits_strategy( uint8_t* byte, void* palette, uint32_t* pixelDataTarget, enum BMP_INFO_HEADER_TYPE headerType, size_t* pixelCounter ){

	uint32_t blue_mask = *( (uint32_t*)palette );
	uint32_t green_mask = *( (uint32_t*)palette + 1 );
	uint32_t red_mask = *( (uint32_t*)palette + 2 );
	uint32_t alpha_mask = ~( red_mask | green_mask | blue_mask );

	uint32_t* dword = (uint32_t*)(byte);
	size_t red = (*dword & red_mask) >> get_dword_mask_offset( &red_mask );
	size_t green = (*dword & green_mask) >> get_dword_mask_offset( &green_mask );
	size_t blue = (*dword & blue_mask) >> get_dword_mask_offset( &blue_mask );
	size_t alpha = (*dword & alpha_mask) >> get_dword_mask_offset( &alpha_mask );

	*((uint8_t*)pixelDataTarget) = red;
	*((uint8_t*)pixelDataTarget + 1) = green;
	*((uint8_t*)pixelDataTarget + 2) = blue;
	*((uint8_t*)pixelDataTarget + 3) = alpha;

	++(*pixelCounter);

}



void* load_image_bmp_strategy(const char* path){
	
	FILE* file = fopen(path, "rb");
	if (file == NULL) return NULL; //err

	//each field is read separately, so as to avoid padding issues

	BMPFileHeader headerData = {0};

	fread(&headerData.sgntr, 2, 1, file);
	fread(&headerData.fsize, 4, 1, file);
	fread(&headerData.res, 4, 1, file);
	fread(&headerData.dataoffset, 4, 1, file);

	printf("File size: %d bytes | Data offset: %x\n", headerData.fsize, headerData.dataoffset);

	uint32_t infoHeaderSize;

	fread(&infoHeaderSize, 4, 1, file);

	enum BMP_INFO_HEADER_TYPE infoHeaderType;
	
	void* infoData = malloc( infoHeaderSize );
	void* palette = NULL;

	switch (infoHeaderSize){
		//40
		default:
			infoHeaderType = BMP_HEADER_DEFAULT;

			BMPInfoHeader* bmpInfoHeader = (BMPInfoHeader*)infoData;
			bmpInfoHeader->infoheadersize = infoHeaderSize;

			fread(&bmpInfoHeader->width, 4, 1, file);
			fread(&bmpInfoHeader->height, 4, 1, file);
			fread(&bmpInfoHeader->planes, 2, 1, file);
			fread(&bmpInfoHeader->bitsperpixel, 2, 1, file);
			fread(&bmpInfoHeader->compression, 4, 1, file);
			fread(&bmpInfoHeader->imagesize, 4, 1, file);
			fread(&bmpInfoHeader->xpixelsperm, 4, 1, file);
			fread(&bmpInfoHeader->ypixelsperm, 4, 1, file);
			fread(&bmpInfoHeader->colorsused, 4, 1, file);
			fread(&bmpInfoHeader->impcolors, 4, 1, file);

			if (bmpInfoHeader->colorsused == 0)
				bmpInfoHeader->colorsused = pow(2, bmpInfoHeader->bitsperpixel);

			if ( bmpInfoHeader->bitsperpixel <= 8 )
				palette = malloc( (infoHeaderType == BMP_HEADER_V5 ? 3 : 4) * bmpInfoHeader->colorsused );
		break;
	}

	BMPInfoHeader* defaultInfoHeader = (BMPInfoHeader*)infoData;

	printf("BMP Info | Header size: %d | Width: %d, Height: %d | Compression: %d | Planes: %d | Colors used: %d | Bits per pixel: %d\n", defaultInfoHeader->infoheadersize, defaultInfoHeader->width, defaultInfoHeader->height,
			defaultInfoHeader->compression, defaultInfoHeader->planes, defaultInfoHeader->colorsused, defaultInfoHeader->bitsperpixel);

	if (defaultInfoHeader->bitsperpixel <= 8){
		fseek(file, 14+infoHeaderSize, SEEK_SET);
		size_t colorsread = fread(palette, infoHeaderType == BMP_HEADER_V5 ? 3 : 4, defaultInfoHeader->colorsused, file );
	}

	//process pixel data
	const size_t pixels = defaultInfoHeader->width*defaultInfoHeader->height;
       	void* rawData = NULL;
	void* pixelData = malloc( 4*pixels );

	size_t bytesPerRow, rowPaddingInBytes, rawDataLength, bytesPerStep;
	
	switch (defaultInfoHeader->compression){
		case 1:

			rawDataLength = defaultInfoHeader->imagesize;
			rawData = malloc( rawDataLength );

			fseek(file, headerData.dataoffset, SEEK_SET);
			fread(rawData, rawDataLength, 1, file);

			bmp_decode_rle_8bit_strategy( 
				rawData, 
				pixelData, 
				palette, 
				defaultInfoHeader->width,
				defaultInfoHeader->height,
				defaultInfoHeader->imagesize, 
				infoHeaderType );
			break;
		case 3:
			palette = malloc( 12 );
		
			fseek(file, 14+infoHeaderSize, SEEK_SET);
			size_t colorsread = fread(palette, 4, 3, file);
		//0 = RGB
		default:
			bytesPerRow = ceil( (double)defaultInfoHeader->width*defaultInfoHeader->bitsperpixel/8.0 );
			rowPaddingInBytes = ceil( (double)bytesPerRow/4.0 )*4 - bytesPerRow;
			
			rawDataLength = defaultInfoHeader->height * (bytesPerRow + rowPaddingInBytes);
			rawData = malloc( rawDataLength );

			fseek(file, headerData.dataoffset, SEEK_SET);
			fread(rawData, rawDataLength, 1, file);

			void (*byte_strategy)(uint8_t*, void*, uint32_t*, enum BMP_INFO_HEADER_TYPE, size_t*) = NULL;

			switch (defaultInfoHeader->bitsperpixel){

				case 1: ;
					byte_strategy = &bmp_raw_data_unpack_1bit_strategy;
					bytesPerStep = 1;
				        break;
				case 4: ;
					byte_strategy = &bmp_raw_data_unpack_4bits_strategy;
					bytesPerStep = 1;
					break;
				case 8: ;
					byte_strategy = &bmp_raw_data_unpack_8bits_strategy;
					bytesPerStep = 1;
					break;
				case 24: ;
					byte_strategy = &bmp_raw_data_unpack_24bits_strategy;
					bytesPerStep = 3;
					break;
				case 32: ;
					byte_strategy = &bmp_raw_data_unpack_32bits_strategy;
					bytesPerStep = 4;
					break;
				default:
					break;

			}

			printf("Loading pixel data (%d bytes):\n", rawDataLength);
			printf("Padding: %d bytes | Used bytes per row: %d | Image size: %d\n", rowPaddingInBytes, bytesPerRow, rawDataLength);
				
			size_t currentPixel = 0;
			//per-row
			for (int row = defaultInfoHeader->height - 1; row >= 0; --row){
				for (size_t rowIteration = 0; rowIteration < bytesPerRow / bytesPerStep; ++rowIteration){

					uint8_t* currentByte = (uint8_t*)rawData + row * ( bytesPerRow + rowPaddingInBytes ) + rowIteration * bytesPerStep;
					uint32_t* pixelDataTarget = (uint32_t*)pixelData + currentPixel;

					byte_strategy( currentByte, palette, pixelDataTarget, infoHeaderType, &currentPixel );
				}
			}					
					
				

			break;
	}

	printf("First, length-1nth and last pixels:\n");
	size_t first = 0, secondLast = pixels - 2, last = pixels - 1;
	uint32_t firstPixel = *( (uint32_t*)pixelData + first );
	uint32_t secondLastPixel = *( (uint32_t*)pixelData + secondLast );
	uint32_t lastPixel = *( (uint32_t*)pixelData + last );

	size_t blueFirst = firstPixel >> 16 & 255, greenFirst = firstPixel >> 8 & 255, redFirst = firstPixel & 255;
	size_t blueSecondLast = secondLastPixel >> 16 & 255, greenSecondLast = secondLastPixel >> 8 & 255, redSecondLast = secondLastPixel & 255;
	size_t blueLast = lastPixel >> 16 & 255, greenLast = lastPixel >> 8 & 255, redLast = lastPixel & 255;

	printf("%d %d %d\n", redFirst, greenFirst, blueFirst );	
	printf("%d %d %d\n", redSecondLast, greenSecondLast, blueSecondLast);
	printf("%d %d %d\n", redLast, greenLast, blueLast);

	/*printf("Displaying pixel data:\n");
	for (size_t y = 0; y < defaultInfoHeader->height; ++y){

		for (size_t x = 0; x < defaultInfoHeader->width; ++x){
			size_t index = y*defaultInfoHeader->width + x;
			uint32_t currentPixel = *((uint32_t*)pixelData + index);
			size_t blue = currentPixel >> 16 & 255, green = currentPixel >> 8 & 255, red = currentPixel & 255;

			printf("%c", light_level_to_fragment( fmin(255, sqrt( red*red + green*green + blue*blue ) ) ));
		}
		printf("\n");
	}*/

	if (rawData != NULL) free(rawData);
	if (palette != NULL) free(palette);
	if (infoData != NULL) free(infoData);	
	return pixelData;
}
