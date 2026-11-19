#ifndef HTNASSETMANAGER_H
#define HTNASSETMANAGER_H

#include <string>
#include "HTNCustomData.h"

class AssetManager
{
public:
	AssetManager() {};
	void AddImage(std::string path, std::string archivePath);
	HTNImgData LoadImage(std::string name, std::string archivePath);
};


#endif