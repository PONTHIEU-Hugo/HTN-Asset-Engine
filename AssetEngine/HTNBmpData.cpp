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

    HTNImgData hid;

    hid.width = (uint32_t)bdh.biWidth;
    hid.height =(uint32_t)bdh.biHeight;
   
    const int size = bdh.biWidth * bdh.biHeight;

    std::vector<BMP_Pixel<uint8_t>> BMPpixels;
    BMPpixels.resize(size);


    file.read(reinterpret_cast<char*>(BMPpixels.data()), sizeof(BMP_Pixel<uint8_t>) * size);

    for (BMP_Pixel<uint8_t> pixelData : BMPpixels)
    {
        hid.pixels.push_back({ pixelData.r , pixelData.g , pixelData.b , 1 });
    }

    return hid;
}
