#ifndef COMPRESS_H
#define COMPRESS_H

#include <zlib.h>
#include <string>

namespace htn
{
	struct Compress
	{
		static uLong current_comp_size;

		static std::string GetDeflatted(char* _input, size_t size);
		static std::string GetInflatted(char* _input, size_t size, size_t compressed_size);
	};
}

#endif