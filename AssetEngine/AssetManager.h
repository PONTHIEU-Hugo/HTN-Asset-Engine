#ifndef HTNASSETMANAGER_H
#define HTNASSETMANAGER_H

#include <string>
#include "HTNCustomData.h"

#define EMPTY_SPACE_BSIZE 44

class AssetManager
{
public:
	static void AddImage(std::string path, std::string archivePath);
	static void RemoveImage(std::string name, std::string archivePath);
	static HTNImgData LoadImage(std::string name, std::string archivePath);
};


#endif