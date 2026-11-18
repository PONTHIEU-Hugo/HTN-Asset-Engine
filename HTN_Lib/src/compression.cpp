#include "compression.h"
#include "../zlib/zlib.h"

namespace htn
{
	uLong Compress::current_comp_size = 0;

	std::string Compress::GetDeflatted(char* _input, size_t size)
	{
        std::string output = {};
        output.resize(size);

        z_stream defstream;
        defstream.zalloc = Z_NULL;
        defstream.zfree = Z_NULL;
        defstream.opaque = Z_NULL;
        defstream.avail_in = (uInt)size; // size of input
        defstream.next_in = (Bytef*)_input; // input char array
        defstream.avail_out = (uInt)size; // size of output
        defstream.next_out = (Bytef*)output.data(); // output char array

        deflateInit(&defstream, Z_DEFAULT_COMPRESSION);
        deflate(&defstream, Z_FINISH);
        deflateEnd(&defstream);

        current_comp_size = defstream.total_out;
        output.resize(current_comp_size);

        return output;
	}

    uint64_t Compress::GetDeflattedBound(char *input, size_t size)
    {
        return 0;
    }

    std::string Compress::GetInflatted(char* _input, size_t size, size_t compressed_size)
	{
        std::string output = {};
        output.resize(size);

        z_stream infstream;
        infstream.zalloc = Z_NULL;
        infstream.zfree = Z_NULL;
        infstream.opaque = Z_NULL;
        infstream.avail_in = (uInt)compressed_size; // size of input
        infstream.next_in = (Bytef*)_input; // input char array
        infstream.avail_out = (uInt)size; // size of output
        infstream.next_out = (Bytef*)output.data(); // output char array

        inflateInit(&infstream);
        inflate(&infstream, Z_NO_FLUSH);
        inflateEnd(&infstream);

        return output;
	}
}