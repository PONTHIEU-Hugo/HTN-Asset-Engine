#pragma once
#include <cstdint>
#include <string>
#include <vector>

struct HTN_Pixel
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};

//TODO REMOVE
struct HTNImgData
{
	uint32_t width;
	uint32_t height;
	std::vector<std::vector<HTN_Pixel>> pixels = {};
};

#pragma pack(push, 1)
struct TgaColorMapSpe
{
	uint16_t indexFirstColorMapEntry;
	uint16_t numEntriesOfColorMap;
	uint8_t numBitsPerColorEntry;
};

struct TgaImageSpe
{
	uint16_t xOrigin;
	uint16_t yOrigin;

	uint16_t imgWidth;
	uint16_t imgHeight;

	uint8_t pixelDepth;
	uint8_t imgDesc;
};

struct TgaHeader
{
	uint8_t idLength;
	uint8_t colorMapType;
	uint8_t imageType;

	TgaColorMapSpe colorMapSpe;
	TgaImageSpe imgSpe;
};

struct TgaPixel24
{
	uint8_t b;
	uint8_t g;
	uint8_t r;
};

struct TgaPixel32
{
	uint8_t b;
	uint8_t g;
	uint8_t r;
	uint8_t a;
};
#pragma pack(pop)

class TgaLoader
{
public:
	static HTNImgData Load(std::string const& path);
};

