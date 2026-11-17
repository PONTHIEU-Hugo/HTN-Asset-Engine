#include "crc.h"

namespace htn
{
	CRC32 Crc::table[256] = {};

	CRC32 Crc::Get(const uint8_t data[], size_t data_length)
	{
		CRC32 crc32 = 0xFFFFFFFFu;

		if (table[255] == 0)
			Init();

		for (size_t i = 0; i < data_length; ++i)
		{
			crc32 ^= data[i];
			crc32 = (crc32 >> 8) ^ table[crc32 & 0xFF];
		}

		crc32 ^= 0xFFFFFFFFu;
		return crc32;
	}

	void Crc::Init()
	{
		CRC32 crc32 = 1;

		for (unsigned int i = 128; i; i >>= 1)
		{
			crc32 = (crc32 >> 1) ^ (crc32 & 1 ? 0xedb88320 : 0);
			for (unsigned int j = 0; j < 256; j += 2 * i)
			{
				table[i + j] = crc32 ^ table[j];
			}
		}
	}
}