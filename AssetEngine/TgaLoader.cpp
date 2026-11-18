#include "TgaLoader.h"
#include <fstream>
#include <iostream>

HTNImgData TgaLoader::Load(std::string const& path)
{
	std::ifstream file;
	file.open(path, std::ios::binary | std::ios::in);

	TgaHeader th = {};
	file.read(reinterpret_cast<char*>(&th), sizeof(TgaHeader));

	//read the optional id length
	file.seekg(th.idLength, std::ios::cur);

	//read the color map 
	if(th.colorMapType == 1)
		file.seekg(th.colorMapSpe.numBitsPerColorEntry * th.colorMapSpe.numEntriesOfColorMap, std::ios::cur);

	HTNImgData out = {};
	out.width = th.imgSpe.imgWidth;
	out.height = th.imgSpe.imgHeight;

	const int size = out.width * out.height;
	out.pixels.resize(out.width);
	
	for (std::vector<HTN_Pixel>& h : out.pixels)
	{
		h.resize(out.height);
	}

	if (th.imgSpe.pixelDepth == 24)
	{
		std::vector<TgaPixel24> pixels;
		pixels.resize(size);

		file.read(reinterpret_cast<char*>(pixels.data()), sizeof(TgaPixel24) * size);

		for (int i = 0; i < size; ++i)
		{
			int x = i % out.width;
			int y = (out.height - 1) - i / out.width;

			TgaPixel24& p24 = pixels[i];
			HTN_Pixel& p = out.pixels[x][y];
			p.r = p24.r;
			p.g = p24.g;
			p.b = p24.b;
			p.a = 255;
		}
	}
	else if (th.imgSpe.pixelDepth == 32)
	{
		std::vector<TgaPixel32> pixels;
		pixels.resize(size);

		file.read(reinterpret_cast<char*>(pixels.data()), sizeof(TgaPixel32) * size);

		for (int i = 0; i < size; ++i)
		{
			int x = i % out.width;
			int y = (out.height - 1) - i / out.width;

			TgaPixel32& p32 = pixels[i];
			HTN_Pixel& p = out.pixels[x][y];
			p.r = p32.r;
			p.g = p32.g;
			p.b = p32.b;
			p.a = 255;
		}
	}

	return out;
}
