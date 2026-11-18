#include "HTNBmpData.h"
#include <fstream>


const HTNImgData BMPLoader::Load(std::string const& path)
{
    std::ifstream file;
    file.open(path, std::ios::binary | std::ios::in);

    BMPFileHeader bh;
    BMP_DIB_Header bdh;

    file.read(reinterpret_cast<char*>(&bh), sizeof(BMPFileHeader));
    file.read(reinterpret_cast<char*>(&bdh), sizeof(BMP_DIB_Header));
    file.seekg(bh.imageDataOffset);

    HTNImgData out;
    out.width = (uint32_t)bdh.biWidth;
    out.height = (uint32_t)bdh.biHeight;

    out.pixels.resize(out.width);
    for (std::vector<HTN_Pixel>& h : out.pixels)
    {
        h.resize(out.height);
    }

    const int size = bdh.biWidth * bdh.biHeight;

    std::vector<BMP_Pixel<uint8_t>> BMPpixels;
    BMPpixels.resize(size);

    file.read(reinterpret_cast<char*>(BMPpixels.data()), sizeof(BMP_Pixel<uint8_t>) * size);

    for (int i = 0; i < size; ++i)
    {
        int x = i % out.width;
        int y = (out.height - 1) - i / out.width;

        BMP_Pixel<uint8_t>& bmpP = BMPpixels[i];
        HTN_Pixel& p = out.pixels[x][y];
        p.r = bmpP.r;
        p.g = bmpP.g;
        p.b = bmpP.b;
        p.a = 255;
    }

    return out;
}
