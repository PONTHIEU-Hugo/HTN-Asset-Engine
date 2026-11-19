#include "AssetEngine.h"
#include "AssetManager.h"
using namespace htn;

int main()
{
	AssetManager asset;
	asset.AddImage("C:/Users/nflesch/Documents/GitHub/AssetEngine/res/chat.bmp","archive");
	// std::string path("D:/source/Cours GTECH3/Asset-Engine/AssetEngine/");
	// std::string testPath(path + "test.htn");
	// Archive test(testPath);
	// std::string chatPath(path + "chat.bmp");
	// test.AddFile(chatPath);

	std::string path("H:/source/Cours GTECH3/Asset-Engine/AssetEngine/");
	std::string testPath(path + "test.htn");
	Archive test(testPath);
	std::string chatPath(path + "chat.bmp");
	test.AddFile(chatPath);
	std::string ppPath(path + "pp.bmp");
	test.AddFile(ppPath);
	
	File out(path + "Return.bmp");
	test.ReadFile("pp.bmp", out.GetData());
	out.SaveData();

	return 0;
}