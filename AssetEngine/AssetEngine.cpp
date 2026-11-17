#include "AssetEngine.h"
COMMANDLINE_H

int main(int argc, char* argv[])
{
	std::string path(argv[0]);
	std::vector<std::string> args = {};

	htn::commandLineExe cLineExe;
	
	for (int i = 1; i < argc; ++i)
	{
		std::string arg(argv[i]);
		args.push_back(arg);
	}

	cLineExe.CreateCommandOrder(args);
	return 0;
}
