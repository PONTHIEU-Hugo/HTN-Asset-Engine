#include "AssetManager.h"
#include <fstream>
#include <iostream>
#include "HTNBmpData.h"

#include <htn/htn.hpp>
#include "TgaLoader.h"
void AssetManager::AddImage(std::string path, std::string archivePath)
{
	std::fstream file (path);
	if (!file.is_open())
	{
		std::cout << "path error : cant read the files " << std::endl;
		return;
	}


	HTNImgData hid;

	if (path.find(".bmp"))
	{
		hid = BMPLoader::Load(path);
	}
	else if (path.find(".tga"))
	{
		hid = TgaLoader::Load(path);
	}
	std::string name;

	for (char c : path)
	{
		if (c == '/')
			name.clear();
		else
			name.push_back(c);
	}

	HTNImgHeader hih = {};
	memcpy(hih.fileName, name.data(), MAX_CHAR_NAME-1);
	hih.height = hid.height;
	hih.width = hid.width;
	hih.dataSize = hih.height * hih.width;
	hih.imageDataOffset = sizeof(HTNImgHeader);

	std::vector <uint8_t> data;
	data.resize(hih.imageDataOffset + hid.width * hid.height * sizeof(HTN_Pixel));


	memcpy(data.data(), &hih, sizeof(HTNImgHeader));

	size_t offset = sizeof(HTNImgHeader);

	for (std::vector<HTN_Pixel>& rowPixel : hid.pixels)
	{
		memcpy(data.data() + offset, rowPixel.data(), hid.height * sizeof(HTN_Pixel));

		offset += hid.height * sizeof(HTN_Pixel);
	}

	htn::Archive archive(archivePath);
	archive.AddFile(data, name);
}

HTNImgData AssetManager::LoadImage(std::string name, std::string archivePath)
{
	std::vector<uint8_t> data = {};
	htn::Archive archive(archivePath);
	archive.ReadFile(name,data);

	if (data.empty())
	{
		std::cout << "the name isnt in the Archive " << std::endl;
		return HTNImgData();
	}

	HTNImgHeader hih = {};
	memcpy(&hih, data.data(), sizeof(HTNImgHeader));

	HTNImgData hid;

	hid.width = hih.width;
	hid.height = hih.height;


	hid.pixels.resize(hih.width);

	size_t offset = sizeof(HTNImgHeader);

	for (int i = 0 ; i < hih.width;i++)
	{
		hid.pixels[i].resize(hih.height);

		memcpy(hid.pixels[i].data(), data.data() + offset, hih.height * sizeof(HTN_Pixel));

		offset += hih.height * sizeof(HTN_Pixel);
	}

	return hid;
}
