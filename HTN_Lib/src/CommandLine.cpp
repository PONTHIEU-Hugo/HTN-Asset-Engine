#include "CommandLine.h"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include "headers.h"

#include "crypt.h"

namespace htn
{
	void CommandLine::CreateCommandOrder(std::vector<std::string> args)
	{
		if (args.empty())
			return;

		if (!HandleCommand(args))
			std::cerr << "Syntax error, type help for information" << std::endl;
	}

	bool CommandLine::HandleCommand(std::vector<std::string> args)
	{
		std::string action = args[0];

		bool found = true;

		if (action == "createArchive")
		{
			if (args.size() != 2)
			{
				std::cerr << "2 arguments required !\n";
				return false;
			}

			std::string path = args[1];
			if (File::IsFileExists(path, ".htn"))
			{
				std::cout << "This archive already exists at " << path << std::endl;
				return false;
			}

			Archive t(path);
			std::cout << "Archive created at " << path << std::endl;
		}
		else if (action == "addFiles")
		{
			if (args.size() < 3)
			{
				std::cerr << "Minimum 3 arguments required !\n";
				return false;
			}

			std::string archive_path = args[1];

			if (!File::IsFileExists(archive_path, ".htn"))
			{
				std::cout << "Archive not found at " << archive_path << std::endl;
				return false;
			}

			Archive t(archive_path);
			
			for (int i = 2; i < args.size(); ++i)
			{
				std::string file_path = args[i];

				if (!File::IsFileExists(file_path))
				{
					std::cerr << "File not found at " << archive_path << std::endl;
					continue;
				}

				t.AddFile(file_path);

				std::cout << "Successfully added " << file_path << " at " << archive_path << std::endl;
			}
		}
		else if (action == "removeFiles")
		{
			if (args.size() < 3)
			{
				std::cerr << "Minimum 3 arguments required !\n";
				return false;
			}

			std::string archive_path = args[1];
		
			if (!File::IsFileExists(archive_path, ".htn"))
			{
				std::cout << "Archive not found at " << archive_path << std::endl;
				return false;
			}

			Archive t(archive_path);

			for (int i = 2; i < args.size(); ++i)
			{
				std::string file_name = args[i];

				if (t.RemoveFile(file_name))
				{
					std::cout << "Successfully removed " << file_name << std::endl;
				}
				else
				{
					std::cout << "Failed to remove " << file_name << std::endl;
				}
			}
		}
		else if (action == "getFileNames")
		{
			if (args.size() != 2)
			{
				std::cerr << "2 arguments required !\n";
				return false;
			}

			std::string archive_path = args[1];

			if (!File::IsFileExists(archive_path, ".htn"))
			{
				std::cout << "Archive not found at " << archive_path << std::endl;
				return false;
			}

			Archive t(archive_path);
			std::vector<std::string> fnames = t.GetFileNames();

			std::cout << "Files count at " << archive_path << " = " << fnames.size() << std::endl;

			for (std::string& n : fnames)
			{
				std::cout << n << std::endl;
			}
		}
		else if (action == "extractAllFiles")
		{
			if (args.size() != 3)
			{
				std::cerr << "3 arguments required !\n";
				return false;
			}

			std::string archive_path = args[1];
			std::string out_path = args[2];

			if (!File::IsFileExists(archive_path, ".htn"))
			{
				std::cout << "Archive not found at " << archive_path << std::endl;
				return false;
			}

			Archive t(archive_path);
			std::vector<std::string> fnames = t.GetFileNames();

			for (std::string& n : fnames)
			{
				File f(out_path + n);

				if (t.ReadFile(n, f.GetData()))
				{
					f.SaveData();
					std::cout << "Successfully extracted " << n << std::endl;
				}
				else
				{
					std::cout << "Failed to extract " << n << std::endl;
				}
			}
			
		}
		else if (action == "extractFile")
		{
			if (args.size() != 4)
			{
				std::cerr << "4 arguments required !\n";
				return false;
			}

			std::string archive_path = args[1];
			std::string file_name = args[2];
			std::string out_path = args[3];

			if (!File::IsFileExists(archive_path, ".htn"))
			{
				std::cout << "Archive not found at " << archive_path << std::endl;
				return false;
			}

			Archive t(archive_path);
			File f(out_path + file_name);

			if (t.ReadFile(file_name, f.GetData()))
			{
				f.SaveData();
				std::cout << "Successfully extracted " << file_name << std::endl;
			}
			else
			{
				std::cout << "Failed to extract " << file_name << std::endl;
				return false;
			}
		}
		else if (action == "encrypt")
		{
			if (args.size() < 3)
			{
				std::cerr << "Minimum 3 arguments required !\n";
				return false;
			}

			std::string in_path = args[1];
			std::string key = args[2];

			std::string out_path = "";
			if (args.size() == 4)
			{
				out_path = args[3];
			}

			if (!File::IsFileExists(in_path))
			{
				std::cout << "File not found at " << in_path << std::endl;
				return false;
			}

			File f(in_path);
			std::cout << "Encrypting...\n";
			f.Encrypt(key, out_path);

			std::cout << "Successfuly encrypted File at " << in_path << std::endl;
		}
		else if (action == "decrypt")
		{
			if (args.size() < 3)
			{
				std::cerr << "Minimum 3 arguments required !\n";
				return false;
			}

			std::string in_path = args[1];
			std::string key = args[2];

			std::string out_path = "";
			if (args.size() == 4)
			{
				out_path = args[3];
			}

			if (!File::IsFileExists(in_path))
			{
				std::cout << "File not found at " << in_path << std::endl;
				return false;
			}

			File f(in_path);
			std::cout << "Decrypting...\n";
			f.Decrypt(key, out_path);

			std::cout << "Successfuly decrypted File at " << in_path << std::endl;
		}
		else if (action == "help")
		{
			std::string txt =
				"- GUIDE TO USE THE HTN LIB -\n\n"
				"Actions : \n\n"
				"createArchive {pathAndName}\n"
				"-> createArchive 'reallycoolpath/archive_name.htn'\n\n"

				"addFiles {pathArchive} {filePath}... {otherFilePath}\n"
				"-> addFiles 'reallycoolpath/archive_name.htn' 'path2/file_name.bmp' 'path/truc.bmp'\n\n"

				"removeFiles {pathArchive} {fileName}... {otherFileName}\n"
				"-> removeFiles 'reallycoolpath/archive_name.htn' 'file_name.bmp' 'truc.bmp'\n\n"

				"getFileNames {pathArchive}\n"
				"-> getFileNames 'reallycoolpath/archive_name.htn'\n\n"

				"extractAllFiles {pathArchive} {outDir}\n"
				"-> extractAllFiles 'reallycoolpath/archive_name.htn' 'superDir/'\n\n"

				"extractFile {pathArchive} {fileName} {outDir}\n"
				"-> extractFile 'reallycoolpath/archive_name.htn' 'file_name.bmp' 'superDir/'\n\n"

				"encrypt {pathFile} {key} {outDir = optional}\n"
				"-> encrypt 'path/file.txt' 'bestkeyever' 'path2/'\n"
				"-> encrypt 'path/file.txt' 'bestkeyever'\n\n"

				"decrypt {pathFile} {key} {outDir = optional}\n"
				"-> decrypt 'path/file.txt' 'bestkeyever' 'path2/'\n"
				"-> decrypt 'path/file.txt' 'bestkeyever'\n\n"
				;
				
			std::cout << txt << std::endl;
		}
		else
			found = false;

		return found;
	}
}
