#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <vector>
#include <string>
#include <unordered_map>
namespace htn
{

	struct CommandOrder
	{
		int commandType;
		std::string path;
		std::string newName;
	};

	struct commandLineExe
	{
		commandLineExe();
		std::unordered_map<std::string, int > Actions;
		std::vector<CommandOrder> commandOrders;
		void CreateCommandOrder();
		//void UpdateCommandLine();
		bool AnalyseCommandOrder(std::string commandText);
	
	};
}


#endif