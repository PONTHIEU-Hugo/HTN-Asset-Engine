#include "compress.h"
#include <iostream>

namespace htn
{
	std::string Compress::GetDeflatted(uint8_t* input, size_t size)
	{
			std::string output = {};
			output.resize(size);

			z_stream defstream;
			defstream.zalloc = Z_NULL;
			defstream.zfree = Z_NULL;
			defstream.opaque = Z_NULL;
			defstream.avail_in = (uInt)size; // size of input
			defstream.next_in = (Bytef*)input; // input char array
			defstream.avail_out = (uInt)size; // size of output
			defstream.next_out = (Bytef*)output.data(); // output char array

			deflateInit(&defstream, Z_DEFAULT_COMPRESSION);
			if(deflate(&defstream, Z_FINISH) != Z_STREAM_END)
					throw std::runtime_error("Deflate didn't ended well.");
			deflateEnd(&defstream);

			current_comp_size = defstream.total_out;
			output.resize(current_comp_size);

			return output;
	}

    uLong Compress::GetDeflattedBound(uint8_t* input, size_t size)
    {
        z_stream strm;
        deflateInit(&strm, Z_DEFAULT_COMPRESSION);
        strm.next_in = input;
        strm.avail_in = size;

		uLong bound = deflateBound(&strm, size);
		deflateEnd(&strm);
        return bound;
    }

    std::string Compress::GetInflatted(uint8_t* input, size_t size, size_t compressed_size)
	{
        std::string output = {};
        output.resize(size);

        z_stream infstream;
        infstream.zalloc = Z_NULL;
        infstream.zfree = Z_NULL;
        infstream.opaque = Z_NULL;
        infstream.avail_in = (uInt)compressed_size; // size of input
        infstream.next_in = (Bytef*)input; // input char array
        infstream.avail_out = (uInt)size; // size of output
        infstream.next_out = (Bytef*)output.data(); // output char array

        inflateInit(&infstream);
        if(inflate(&infstream, Z_FINISH) != Z_STREAM_END)
                throw std::runtime_error("Inflate didn't ended well.");
        inflateEnd(&infstream);

        return output;
	}
}