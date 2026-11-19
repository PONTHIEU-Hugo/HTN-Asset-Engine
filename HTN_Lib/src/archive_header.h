#ifndef ARCHIVE_HEADER_H
#define ARCHIVE_HEADER_H

#include <stdint.h>
#include <string>
#include "CompressionInfo.h"

#define MAX_CHAR_NAME 32

namespace htn
{
	using offset_t = uint64_t;

	#pragma pack(push, 1)

	struct ArchiveHeader
	{
		CompressionInfo compressionInfo = { 0, 0, 0 };				// Compression info of the footer block
		offset_t firstFooterPos 		= sizeof(ArchiveHeader);	// first footer position
	};

	struct ArchiveFileFooter
	{
		CompressionInfo compressionInfo = { 0, 0, 0 };	// Compression of the associated file

		offset_t dataPos;
		char name[MAX_CHAR_NAME];
		
		offset_t nextFooterPos;
	};

	#pragma pack(pop)
}

#endif