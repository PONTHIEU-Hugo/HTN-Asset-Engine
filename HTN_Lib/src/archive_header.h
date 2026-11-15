#ifndef ARCHIVE_HEADER_H
#define ARCHIVE_HEADER_H

#include <stdint.h>

#define MAX_CHAR_NAME 32

using offset_t = uint32_t;

namespace htn
{
	#pragma pack(push, 1)

	struct ArchiveHeader
	{
		uint32_t crc32;
		offset_t firstHeaderPos;
	};

	struct ArchiveFileHeader
	{
		uint32_t crc32;
		uint32_t compressFileSize;

		offset_t dataPos;
		offset_t nextHeaderPos;

		char name[MAX_CHAR_NAME];
	};

	#pragma pack(pop)
}

#endif