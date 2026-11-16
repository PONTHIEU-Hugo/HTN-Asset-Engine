#include "archive_header.h"

namespace htn
{
    using CRC32 = uint32_t;

    #pragma pack(push, 1)
    struct CompressionInfo
    {
        CRC32 crc32                 = 0;
        uint64_t compressedSize     = 0;
        uint64_t decompressedSize   = 0;
    };
    #pragma pack(pop)
}