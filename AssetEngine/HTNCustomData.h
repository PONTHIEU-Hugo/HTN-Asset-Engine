#ifndef HTNCUSTOMDATA_H
#define HTNCUSTOMDATA_H

#include <iostream>
#include <vector>

#pragma pack(push ,1)
struct HTNImgHeader
{
	unsigned char fileName[32];
	uint32_t width;
	uint32_t height;
	uint64_t dataSize;
	uint64_t imageDataOffset;
};

struct HTN_Pixel
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};

#pragma pack (pop)


struct HTNImgData
{
	uint32_t width;
	uint32_t height;
	std::vector<std::vector<HTN_Pixel>> pixels = {};
};
#endif