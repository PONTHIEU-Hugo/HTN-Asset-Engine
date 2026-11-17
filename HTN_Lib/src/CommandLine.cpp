#include "CommandLine.h"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include "headers.h"



bool htn::commandLineExe::VerifyAction(std::string order )
{
	if (order == "create")
		return true;
	else if (order == "addFile")
		return true;
	else if (order == "removeFile")
		return true;
	else if (order == "encrypt")
		return true;
	else if (order == "decrypt")
		return true;
	else return false;
}

bool htn::commandLineExe::CreateCommandOrder(std::vector<std::string> args)
{
	if (args.empty())
		return false;
	std::cout << "Action  : " << args[0] << std::endl;

	if (!VerifyAction(args[0]))
	{
		std::cout << "Lexic Error :  Order doesnt Exist" << std::endl;
		return false;
	}

	std::cout << " path : " << args[1] << std::endl;

	std::string optArg  = "";
	if (args.size() > 2)
	{
		optArg = args[2];
	}

	std::cout << "optional argument : " << optArg << std::endl;

	m_commandOrder = { args[0],args[1], optArg };

	return true;
}
