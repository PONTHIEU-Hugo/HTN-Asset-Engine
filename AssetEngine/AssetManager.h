#ifndef HTNASSETMANAGER_H
#define HTNASSETMANAGER_H

#include <string>
#include "HTNCustomData.h"

#define EMPTY_SPACE_BSIZE 44

class AssetManager
{
public:
	static void AddImages(std::string archivePath, std::vector<std::string> imgPaths);
	static void RemoveImages(std::string archivePath, std::vector<std::string> imgNames);
	static HTNImgData LoadImage(std::string archivePath, std::string name);
};


#endif