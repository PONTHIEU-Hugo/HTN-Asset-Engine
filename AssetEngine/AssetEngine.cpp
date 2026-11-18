#include "AssetEngine.h"

using namespace htn;

int main()
{
	std::string path("D:/source/Cours GTECH3/Asset-Engine/AssetEngine/");
	std::string testPath(path + "test.htn");
	Archive test(testPath);
	std::string chatPath(path + "chat.bmp");
	test.AddFile(chatPath);

	return 0;
}
