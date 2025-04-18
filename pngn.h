#pragma once
#ifndef PNGN_DEF
#define PNGN_DEF

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <zlib.h>


typedef struct PNGchunk {
	unsigned int length;
	unsigned char type[5];
	unsigned char* data;
	unsigned char crc[5];

} PNGchunk;

typedef struct RECT {
	int top;
	int bottom;
	int left;
	int right;

};

typedef struct PNG {
	PNGchunk* chunks;
	PNGchunk* IHDRchunk;
	PNGchunk* IDATchunk;
	short int chunkCount;
	RECT dimensions;

} PNG;

inline unsigned int reverseEndian(unsigned int value) {

	unsigned int converted = 0;
	converted |= ((0xff & value) << 24);
	converted |= (((0xff << 8) & value) << 8);
	converted |= (((0xff << 16) & value) >> 8);
	converted |= (((0xff << 24) & value) >> 24);

	return converted;
}

// UNFINISHED !!!
inline PNGchunk getRawIDAT(char* filepath) {

	PNGchunk IDATchunk;
	IDATchunk.length = 0;
	memset(&IDATchunk.type, 0, 4);

	// open file
	FILE* pngfile = fopen(filepath, "rb");

	// look for idat chunk
	while (strcmp(IDATchunk.type, "IDAT")) {
		fread(IDATchunk.type, 1, 4, pngfile);
		fseek(pngfile, -3, SEEK_CUR);
		IDATchunk.type[4] = '\0';
	}

	// go back to read the chunk length
	fseek(pngfile, -5, SEEK_CUR);
	fread(&IDATchunk.length, 4, 1, pngfile);

	// reverse the endianness of the length
	IDATchunk.length = reverseEndian(IDATchunk.length);

	// read the idat data
	IDATchunk.data = malloc(IDATchunk.length);
	fread(IDATchunk.data, 1, IDATchunk.length, pngfile);

	// close the file
	fclose(pngfile);

	return IDATchunk;
}

inline PNG getPNGdata(char* filepath) {

	FILE* pngfile = fopen(filepath, "rb");

	// get filesize
	fseek(pngfile, 0, SEEK_END); // seek to end of file
	int size = ftell(pngfile); // get current file pointer
	fseek(pngfile, 0, SEEK_SET);

	unsigned char signature[8];
	fread(signature, 1, 8, pngfile);

	PNG png = { 0 };

	png.chunks = malloc(sizeof(PNGchunk) * (size - 8) / 12);

	unsigned char* raw_scanlines = "";

	for (int i = 0; i < (size - 8) / 12; i++) {

		fread(&png.chunks[i].length, 4, 1, pngfile);

		png.chunks[i].length = reverseEndian(png.chunks[i].length);

		fread(png.chunks[i].type, 1, 4, pngfile);
		png.chunks[i].type[4] = '\0';

		if (png.chunks[i].length != 0) {
			if (!strcmp(png.chunks[i].type, "IHDR")) {

				fread(&png.dimensions.right, 4, 1, pngfile);
				png.dimensions.right = reverseEndian(png.dimensions.right);

				fread(&png.dimensions.bottom, 4, 1, pngfile);
				png.dimensions.bottom = reverseEndian(png.dimensions.bottom);

				png.chunks[i].data = malloc(sizeof(unsigned char) * png.chunks[i].length);
				fseek(pngfile, -8, SEEK_CUR);
				fread(png.chunks[i].data, 1, png.chunks[i].length, pngfile);

				png.IHDRchunk = &png.chunks[i];
			}
			else if (!strcmp(png.chunks[i].type, "IDAT")) {
				png.chunks[i].data = malloc(sizeof(unsigned char) * png.chunks[i].length);
				fread(png.chunks[i].data, 1, png.chunks[i].length, pngfile);

				png.IDATchunk = &png.chunks[i];
			}
			else {
				png.chunks[i].data = malloc(sizeof(unsigned char) * png.chunks[i].length);
				fread(png.chunks[i].data, 1, png.chunks[i].length, pngfile);
			}
		}

		fread(png.chunks[i].crc, 1, 4, pngfile);
		png.chunks[i].crc[4] = '\0';

		if (!strcmp(png.chunks[i].type, "IEND")) {
			png.chunkCount = i + 1;
			break;
		}

	}

	png.chunks = realloc(png.chunks, sizeof(PNGchunk) * png.chunkCount);

	fclose(pngfile);

	return png;
}

inline void printPNGchunks(PNG png) {
	for (int i = 0; i < png.chunkCount; i++) {
		printf("\nLength: %d\nType: %s\nData: ", png.chunks[i].length, png.chunks[i].type);
		for (int j = 0; j < png.chunks[i].length; j++) {
			printf("%02x ", png.chunks[i].data[j]);
		}
		printf("\nCRC: ");
		for (int j = 0; j < 4; j++) {
			printf("%02x ", png.chunks[i].crc[j]);
		}
		printf("\n");
	}
}

inline unsigned char* getDecompressedPixelData(PNG png) {
	size_t raw_size = (png.dimensions.right * 4 + 1) * png.dimensions.bottom; // Each row has 1 filter byte + (width * 4) pixels
	uint8_t* decompressed_data = malloc(raw_size);
	if (!decompressed_data) {
		fprintf(stderr, "Memory allocation failed.\n");
		return NULL;
	}

	// Set up zlib stream
	z_stream stream = { 0 };
	stream.next_in = (*png.IDATchunk).data;
	stream.avail_in = (*png.IDATchunk).length;
	stream.next_out = decompressed_data;
	stream.avail_out = raw_size;

	if (inflateInit2(&stream, 15 + 32) != Z_OK) {
		fprintf(stderr, "inflateInit2 failed: %s\n", stream.msg);
		free(decompressed_data);
		return NULL;
	}

	int ret = 1;
	do {
		ret = inflate(&stream, Z_NO_FLUSH);
		//printf("inflate step: ret = %d, avail_out = %u, avail_in = %u\n", ret, stream.avail_out, stream.avail_in);
		//if (stream.avail_out == 0) {
		//	stream.next_out = realloc(stream.next_out, raw_size * 2);
		//	stream.avail_out = raw_size * 2;
		//}
	} while (ret == Z_OK && stream.avail_out > 0);

	inflateEnd(&stream);

	// Allocate final RGBA pixel array (without filter bytes)
	uint8_t* pixel_data = malloc(png.dimensions.right * png.dimensions.bottom * 4);
	if (!pixel_data) {
		fprintf(stderr, "Memory allocation failed for pixel array.\n");
		free(decompressed_data);
		return NULL;
	}

	// Remove PNG filter bytes and apply filter correction
	for (int y = 0; y < png.dimensions.bottom; y++) {
		uint8_t filter_type = decompressed_data[y * (png.dimensions.right * 4 + 1)];
		uint8_t* src_row = &decompressed_data[y * (png.dimensions.right * 4 + 1) + 1]; // Skip filter byte
		uint8_t* dst_row = &pixel_data[y * png.dimensions.right * 4];

		if (filter_type == 0) { // No filter (copy directly)
			memcpy(dst_row, src_row, png.dimensions.right * 4);
		}
		else if (filter_type == 1) { // Sub filter
			for (int x = 0; x < png.dimensions.right * 4; x++) {
				uint8_t left = (x >= 4) ? dst_row[x - 4] : 0;
				dst_row[x] = src_row[x] + left;
			}
		}
		else if (filter_type == 2) { // Up filter
			for (int x = 0; x < png.dimensions.right * 4; x++) {
				uint8_t above = (y > 0) ? pixel_data[(y - 1) * png.dimensions.right * 4 + x] : 0;
				dst_row[x] = src_row[x] + above;
			}
		}
		else {
			fprintf(stderr, "Unsupported filter type: %d\n", filter_type);
			free(decompressed_data);
			free(pixel_data);
			return NULL;
		}
	}

	free(decompressed_data);
	return pixel_data;
}

inline void freePNG(PNG png) {
	for (int i = 0; i < png.chunkCount - 1; i++) {
		free(png.chunks[i].data);
	}

	free(png.chunks);
}

#endif