#ifndef CRC_H
#define CRC_H

#include <stdint.h>

namespace htn
{
	using CRC32 = uint32_t;

	struct Crc
	{
	public:
		static CRC32 table[256];

		static CRC32 Get(const uint8_t data[], size_t data_length);

		static void Init();
	};
}

#endif