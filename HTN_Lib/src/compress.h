#ifndef COMPRESS_H
#define COMPRESS_H

#include <zlib.h>
#include <string>
#include <cstdint>

namespace htn
{
	namespace Compress
	{
		inline uLong current_comp_size = 0;

		std::string GetDeflatted(uint8_t* input, size_t size);
		uLong GetDeflattedBound(uint8_t* input, size_t size);

		std::string GetInflatted(uint8_t* input, size_t size, size_t compressed_size);
	};
}

#endif