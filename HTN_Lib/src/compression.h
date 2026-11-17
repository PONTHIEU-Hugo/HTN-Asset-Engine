#ifndef COMPRESSION_H
#define COMPRESSION_H

#include <string>

namespace htn
{
	struct Compress
	{
		static unsigned long current_comp_size;

		static std::string GetDeflatted(char* _input, size_t size);
		static std::string GetInflatted(char* _input, size_t size, size_t compressed_size);
	};
}

#endif