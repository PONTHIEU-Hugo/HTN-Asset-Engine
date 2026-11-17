#include "AssetEngine.h"

int main(int argc, char* argv[])
{
	std::string path(argv[0]);
	std::vector<std::string> args = {};

	for (int i = 1; i < argc; ++i)
	{
		std::string arg(argv[i]);
		args.push_back(arg);
	}

	return 0;
}
