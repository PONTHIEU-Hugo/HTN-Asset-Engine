#include "AssetManager.h"
#include <fstream>
#include <iostream>
#include "HTNBmpData.h"
#include "HTNCustomData.h"
void AssetManager::AddImage(std::string path, std::string archivePath)
{
	std::fstream file (path);
	if (!file.is_open())
	{
		std::cout << "path error : cant read the files " << std::endl;
		return;
	}

	std::vector<HTN_Pixel> customPixelDatas;

	HTNImgData hid;

	if (path.find(".bmp"));
	{
		hid = BMPLoader::Load(path);

	}
	std::string name;

	for (char c : path)
	{
		if (c == '/')
			name.clear();
		else
			name.push_back(c);
	}

	HTNImgHeader hih ;
	memcpy(hih.fileName, name.data(), 32);
	hih.height = hid.height;
	hih.width = hid.width;
	hih.dataSize = hih.height * hih.width;
	hih.imageDataOffset = sizeof(HTNImgHeader);
}

void AssetManager::LoadImage(std::string name, std::string archivePath)
{
}
