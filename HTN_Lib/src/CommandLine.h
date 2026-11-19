#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <vector>
#include <string>
#include <unordered_map>
#include "Archive.h"

namespace htn
{
	class CommandLine
	{
	public:
		static void CreateCommandOrder(std::vector<std::string> args);
		static bool HandleCommand(std::vector<std::string> args);
	};
}


#endif