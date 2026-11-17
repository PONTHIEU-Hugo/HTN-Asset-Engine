#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <vector>
#include <string>
#include <unordered_map>
namespace htn
{

	struct CommandOrder
	{
		std::string commandType;
		std::string path;
		std::string optArg;
	};

	class commandLineExe
	{
		CommandOrder m_commandOrders;
	public:
		commandLineExe() {};
		bool VerifyAction(std::string order);
		bool CreateCommandOrder(std::vector<std::string> args);
	};
}


#endif