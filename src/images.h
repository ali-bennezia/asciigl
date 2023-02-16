#ifndef IMAGES_HEADERGUARD
#define IMAGES_HEADERGUARD

#include <stdint.h>
#include <stdio.h>

typedef struct BMPFileHeader {
	uint16_t sgntr;
	uint32_t fsize;
	uint32_t res;
	uint32_t dataoffset;
} BMPFileHeader;

typedef struct BMPInfoHeader {
	uint32_t infoheadersize;
	int32_t width;
	int32_t height;
	uint16_t planes;
	uint16_t bitsperpixel;
	uint32_t compression;
	uint32_t imagesize;
	uint32_t xpixelsperm;
	uint32_t ypixelsperm;
	uint32_t colorsused;
	uint32_t impcolors;
} BMPInfoHeader;

enum BMP_INFO_HEADER_TYPE {
	BMP_HEADER_DEFAULT,
	BMP_HEADER_V4,
	BMP_HEADER_V5
};

size_t get_dword_mask_offset( uint32_t* dword );

uint32_t BGRA_to_RGBA( uint32_t data );

void bmp_decode_rle_4bit_strategy( void* data, void* destination, void* palette, int32_t width, int32_t height, size_t dataSizeInBytes, enum BMP_INFO_HEADER_TYPE headerType );
void bmp_decode_rle_8bit_strategy( void* data, void* destination, void* palette, int32_t width, int32_t height, size_t dataSizeInBytes, enum BMP_INFO_HEADER_TYPE headerType );

void bmp_raw_data_unpack_1bit_strategy( uint8_t* byte, void* palette, uint32_t* pixelDataTarget, enum BMP_INFO_HEADER_TYPE headerType, size_t* pixelCounter );
void bmp_raw_data_unpack_4bits_strategy( uint8_t* byte, void* palette, uint32_t* pixelDataTarget, enum BMP_INFO_HEADER_TYPE headerType, size_t* pixelCounter );
void bmp_raw_data_unpack_8bits_strategy( uint8_t* byte, void* palette, uint32_t* pixelDataTarget, enum BMP_INFO_HEADER_TYPE headerType, size_t* pixelCounter );
void bmp_raw_data_unpack_24bits_strategy( uint8_t* byte, void* palette, uint32_t* pixelDataTarget, enum BMP_INFO_HEADER_TYPE headerType, size_t* pixelCounter );
void bmp_raw_data_unpack_32bits_strategy( uint8_t* byte, void* palette, uint32_t* pixelDataTarget, enum BMP_INFO_HEADER_TYPE headerType, size_t* pixelCounter );

void* load_image_bmp_strategy(const char* path);

#endif
