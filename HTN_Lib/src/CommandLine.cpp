#include "CommandLine.h"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include "headers.h"
#define COMPRESS 1528461536

htn::commandLineExe::commandLineExe()
{
	std::cout << "command Lexic : <Action>  <Path>  <NewName>" << std::endl;;

	Actions["compress"] = COMPRESS;
}

void htn::commandLineExe::CreateCommandOrder()
{
	
	std::string message;
	if (!std::getline(std::cin, message)) {
		std::cerr << "Error reading input.\n";
		return ;
	}
	if (AnalyseCommandOrder(message))
	{
		//Execute command
	}
}

bool htn::commandLineExe::AnalyseCommandOrder(std::string commandText)
{
	std::vector<int> start;
	std::vector<int> end;
	for (int i = 0; i < commandText.size(); i++)
	{
		if (commandText[i] == '<')
			start.push_back(i +1);
		else if (commandText[i] == '>')
			end.push_back(i -1);
	}

	if (start.size() < 2)
	{
		std::cout << "Lexic Error : Command Order have less than 2 syntax" << std::endl;
		return false;
	}
	else if (start.size() != end.size())
	{
		std::cout << "Lexic Error : Command Order  have a syntax error " << std::endl;
		return false;
	}
	std::string order = commandText.substr(start[0], end[0]);

	std::cout << "Action  : " << order << std::endl;
	if (Actions.find(order) == Actions.end())
	{
		std::cout << "Lexic Error :  Order doesnt Exist" << std::endl;
		return false;
	}

	std::string path = commandText.substr(start[1], end[1] - start[1]+1);

	std::ifstream file(path);
	/*if (!file.is_open())
	{
		std::cout << "Error : File doesnt exist at the path" << std::endl;
		return false;
	}*/
	std::cout << " path : " << path << std::endl;

	std::string name = commandText.substr(start[2], end[2] - start[2] + 1);
	if (name.size() > MAX_CHAR_NAME)
	{
		std::cout << "Error : name size too big" << std::endl;
		return false;
	}
	std::cout << "name : " << name << std::endl;
	CommandOrder  co = { Actions[order],path,name };
	commandOrders.push_back(co);

}
