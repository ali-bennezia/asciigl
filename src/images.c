/* 

	This file is part of asciigl.

	asciigl is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

	asciigl is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with asciigl. If not, see <https://www.gnu.org/licenses/>.

*/

#include <stdio.h>
#include <string.h>

#include "images.h"
#include "utils.h"

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

uint32_t BGRA_to_RGBA( uint32_t data )
{
	uint32_t out = 0;

	out |= (data & 0x000000FF) << 16;
	out |= (data & 0x0000FF00);
	out |= (data & 0x00FF0000) >> 16;
	out |= (data & 0xFF000000);

	return out;
}

void bmp_decode_rle_4bit_strategy( void* data, void* destination, void* palette, int32_t width, int32_t height, size_t dataSizeInBytes, enum BMP_INFO_HEADER_TYPE headerType )
{
	//pixel data mask
	uint32_t mask = headerType == BMP_HEADER_V5 ? 0x00FFFFFF : 0xFFFFFFFF;

	size_t max = dataSizeInBytes/2, x = 0, y = 0;
	for (size_t i = 0; i < max; ++i){
		uint16_t word = *( (uint16_t*)data + i );
		uint8_t secondByte = word >> 8 & 255, firstByte = word & 255;
		uint8_t highOrderData = secondByte >> 4 & 15, lowOrderData = secondByte & 15;

		if (firstByte > 0){
			uint32_t firstPixelData = BGRA_to_RGBA( *( (uint32_t*)palette + highOrderData ) & mask ),
				 secondPixelData = BGRA_to_RGBA( *( (uint32_t*)palette + lowOrderData ) & mask );
			short odd = firstByte % 2 == 0 ? 0 : 1;
			for (size_t p = 0; p < ceil((double)firstByte/2.0); ++p)
			{
				*( (uint32_t*)destination + ( (height-y-1)*width+x ) ) = firstPixelData;
				++x;
				if (odd == 0 || p != ceil( (double)firstByte/2.0 ) - 1)
				{
					*( (uint32_t*)destination + ( (height-y-1)*width+x ) ) = secondPixelData;
					++x;
				}
			}

		}else{
			switch (secondByte){
				case 0x00:; //eol
					x = 0;
					++y;
					break;
				case 0x01:; //eof
					i = max;
					break;
				case 0x02:; //delta
					++i;
					uint16_t deltaWord = *( (uint16_t*)data + i );
					uint8_t verticalDelta = deltaWord >> 8 & 255, horizontalDelta = deltaWord & 255;
					x += horizontalDelta;
					y += verticalDelta;
					break;
				default:; //absolute
					short odd = secondByte % 2 == 0 ? 0 : 1;
					++i;
					for (size_t p = 0; p < ceil ((double)secondByte / 2.0); ++p){

						uint8_t absoluteByte = *( (uint8_t*)data + i*2 + p );
						uint8_t highOrderData = absoluteByte >> 4 & 15, lowOrderData = absoluteByte & 15;

						uint32_t absolutePixelData = *( (uint32_t*)palette + highOrderData ) & mask;

						*( (uint32_t*)destination + ((height-y-1)*width+x) ) = BGRA_to_RGBA( absolutePixelData );
						++x;


						if ( odd == 0 || p != ( ceil((double)secondByte/2.0) - 1) )
						{
							absolutePixelData = *( (uint32_t*)palette + lowOrderData );
							*( (uint32_t*)destination + ((height-y-1)*width+x) ) = BGRA_to_RGBA( absolutePixelData );
							++x;
						}

					}
					i += ceil( (double)secondByte / 4.0 ) - 1;
					break;
			}
		}	
	}
	
}

void bmp_decode_rle_8bit_strategy( void* data, void* destination, void* palette, int32_t width, int32_t height, size_t dataSizeInBytes, enum BMP_INFO_HEADER_TYPE headerType )
{
	//pixel data mask
	uint32_t mask = headerType == BMP_HEADER_V5 ? 0x00FFFFFF : 0xFFFFFFFF;

	size_t max = dataSizeInBytes/2, x = 0, y = 0;
	for (size_t i = 0; i < max; ++i){
		uint16_t word = *( (uint16_t*)data + i );
		uint8_t secondByte = word >> 8 & 255, firstByte = word & 255;

		if (firstByte > 0){
			uint32_t pixelData = BGRA_to_RGBA( *( (uint32_t*)palette + secondByte ) & mask );

			for (size_t p = 0; p < firstByte; ++p)
			{
				*( (uint32_t*)destination + ( (height-y-1)*width+x ) ) = pixelData;
				++x;
			}

		}else{
			switch (secondByte){
				case 0x00:; //eol
					x = 0;
					++y;
					break;
				case 0x01:; //eof
					i = max;
					break;
				case 0x02:; //delta
					++i;
					uint16_t deltaWord = *( (uint16_t*)data + i );
					uint8_t verticalDelta = deltaWord >> 8 & 255, horizontalDelta = deltaWord & 255;
					x += horizontalDelta;
					y += verticalDelta;
					break;
				default:; //absolute
					short odd = secondByte % 2 == 0 ? 0 : 1, processingSecondByte = 0;
					
					for (size_t p = 0; p < ceil ((double)secondByte / 2.0); ++p){
						++i;
						uint16_t absoluteWord = *( (uint16_t*)data + i );
						uint8_t secondByte = absoluteWord >> 8 & 255, firstByte = absoluteWord & 255;
						processingSecondByte = 0;

						processByte:;
						uint32_t absolutePixelData = *( (uint32_t*)palette + ( processingSecondByte == 0 ? firstByte : secondByte) );
						*( (uint32_t*)destination + ((height-y-1)*width+x) ) = BGRA_to_RGBA( absolutePixelData & mask );

						++x;
	
						if ( processingSecondByte == 0 && ( odd == 0 || p != ( ceil((float)secondByte/2.0) - 1) ) )
						{
							processingSecondByte = 1;
							goto processByte;
						}
					}

					break;
			}
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


//#include "state.h"

void* load_image_bmp_strategy(const char* path, size_t* out_sizeInBytes, size_t* out_width, size_t* out_height){
	
	FILE* file = fopen(path, "rb");
	if (file == NULL) return NULL; //err

	//each field is read separately, so as to avoid padding issues

	BMPFileHeader headerData = {0};

	fread(&headerData.sgntr, 2, 1, file);
	fread(&headerData.fsize, 4, 1, file);
	fread(&headerData.res, 4, 1, file);
	fread(&headerData.dataoffset, 4, 1, file);

	//printf("File size: %d bytes | Data offset: %x\n", headerData.fsize, headerData.dataoffset);

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

	/*printf("BMP Info | Header size: %d | Width: %d, Height: %d | Compression: %d | Planes: %d | Colors used: %d | Bits per pixel: %d\n", defaultInfoHeader->infoheadersize, defaultInfoHeader->width, defaultInfoHeader->height,
			defaultInfoHeader->compression, defaultInfoHeader->planes, defaultInfoHeader->colorsused, defaultInfoHeader->bitsperpixel);*/

	if (defaultInfoHeader->bitsperpixel <= 8){
		fseek(file, 14+infoHeaderSize, SEEK_SET);
		size_t colorsread = fread(palette, infoHeaderType == BMP_HEADER_V5 ? 3 : 4, defaultInfoHeader->colorsused, file );
	}

	//process pixel data
	const size_t pixels = abs(defaultInfoHeader->width*defaultInfoHeader->height);
	*out_sizeInBytes = pixels;
	*out_width = abs( defaultInfoHeader->width );
	*out_height = abs( defaultInfoHeader->height );
       	void* rawData = NULL;
	void* pixelData = calloc( pixels, 4 );

	size_t bytesPerRow, rowPaddingInBytes, rawDataLength, bytesPerStep;
	
	switch (defaultInfoHeader->compression){
		case 1:
		case 2:
			rawDataLength = defaultInfoHeader->imagesize;
			rawData = malloc( rawDataLength );

			fseek(file, headerData.dataoffset, SEEK_SET);
			fread(rawData, rawDataLength, 1, file);
			
			if (defaultInfoHeader->compression == 1)
				bmp_decode_rle_8bit_strategy( 
					rawData, 
					pixelData, 
					palette, 
					defaultInfoHeader->width,
					defaultInfoHeader->height,
					defaultInfoHeader->imagesize, 
					infoHeaderType );
			else
				bmp_decode_rle_4bit_strategy( 
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

			/*printf("Loading pixel data (%d bytes):\n", rawDataLength);
			printf("Padding: %d bytes | Used bytes per row: %d | Image size: %d\n", rowPaddingInBytes, bytesPerRow, rawDataLength);*/
				
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

	//printf("First, length-1nth and last pixels:\n");
	size_t first = 0, secondLast = pixels - 2, last = pixels - 1;
	uint32_t firstPixel = *( (uint32_t*)pixelData + first );
	uint32_t secondLastPixel = *( (uint32_t*)pixelData + secondLast );
	uint32_t lastPixel = *( (uint32_t*)pixelData + last );

	size_t blueFirst = firstPixel >> 16 & 255, greenFirst = firstPixel >> 8 & 255, redFirst = firstPixel & 255;
	size_t blueSecondLast = secondLastPixel >> 16 & 255, greenSecondLast = secondLastPixel >> 8 & 255, redSecondLast = secondLastPixel & 255;
	size_t blueLast = lastPixel >> 16 & 255, greenLast = lastPixel >> 8 & 255, redLast = lastPixel & 255;

	/*printf("%d %d %d\n", redFirst, greenFirst, blueFirst );	
	printf("%d %d %d\n", redSecondLast, greenSecondLast, blueSecondLast);
	printf("%d %d %d\n", redLast, greenLast, blueLast);

	printf("Displaying pixel data:\n");*/
	/*for (size_t y = 0; y < defaultInfoHeader->height; ++y){

		for (size_t x = 0; x < defaultInfoHeader->width; ++x){
			size_t index = y*defaultInfoHeader->width + x;
			uint32_t currentPixel = *((uint32_t*)pixelData + index);
			size_t blue = currentPixel >> 16 & 255, green = currentPixel >> 8 & 255, red = currentPixel & 255;

			//printf("%c", light_level_to_fragment( fmin(255, sqrt( red*red + green*green + blue*blue ) ) ));
		}
		//printf("\n");
	}*/

	if (rawData != NULL) free(rawData);
	if (palette != NULL) free(palette);
	if (infoData != NULL) free(infoData);	
	return pixelData;
}

Texture* load_texture(const char* path)
{
	char signature[3];
	signature[2] = '\0';

	FILE* f = fopen(path, "rb");
	if (f == NULL) return NULL;
	fread(&signature[0], 2, 1, f);
	fclose(f);
	
	if ( strcmp( &signature[0], "BM" ) == 0 )
	{
		Texture* tex = malloc( sizeof( Texture ) );
		tex->data = load_image_bmp_strategy(path, &tex->sizeInBytes, &tex->width, &tex->height);
		return tex;
	}else fprintf(stderr, "Unsupported file type. Signature: %s\n", &signature[0]);

	return NULL;
}

int free_texture( Texture* tex )
{
	if (tex == NULL) return 1;
	if (tex->data != NULL) free(tex->data);
	free(tex);
	return 0;
}

