#ifndef HTNASSETMANAGER_H
#define HTNASSETMANAGER_H

#include <string>

class AssetManager
{
public:
	AssetManager() {};
	void AddImage(std::string path, std::string archivePath);
	void LoadImage(std::string name, std::string archivePath);
};


#endif