#include <iostream>
#include <string>
#include <vector>
#include "include/htn/htn.hpp"

int main(int argc, char* argv[])
{
	std::vector<std::string> args = {};

	for (int i = 1; i < argc; ++i)
	{
		std::string arg(argv[i]);
		args.push_back(arg);
	}

	htn::CommandLine::CreateCommandOrder(args);

	return 0;
}
