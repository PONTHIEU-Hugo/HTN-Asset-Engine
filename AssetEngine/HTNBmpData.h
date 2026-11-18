#ifndef HTNBMPDATA_H
#define HTNBMPDATA_H
#include <iostream>
#include <vector>
#include "HTNCustomData.h"

#pragma pack(push, 1)
struct BMPFileHeader
{
    unsigned char fileMarker[2];
    unsigned int size;
    uint16_t unused1;
    uint16_t unused2;
    unsigned int imageDataOffset;
};

struct BMP_DIB_Header
{
    unsigned int biSize;

    unsigned int biWidth;
    unsigned int biHeight;

    uint16_t biPlanes;
    uint16_t biBitCount;

    unsigned int biCompression;
    unsigned int biSizeImage;
    unsigned int biXPelsPerMeter;
    unsigned int biYPelsPerMeter;
    unsigned int biClrUsed;
    unsigned int biClrImportant;
};

template <typename T>
struct BMP_Pixel
{
    T b;
    T g;
    T r;
};

struct BMPLoader
{
    const static HTNImgData Load(std::string const& path);
};
#endif