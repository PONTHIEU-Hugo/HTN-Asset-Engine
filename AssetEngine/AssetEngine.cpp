#include "AssetEngine.h"

using namespace htn;

int main()
{
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