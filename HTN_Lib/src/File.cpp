#include "File.h"
#include <filesystem>
#include <fstream>
#include <zlib.h>

htn::File::File(std::string filePath) : 
	m_FilePath(filePath)
{
}

void htn::File::ExtractData()
{
	m_Data.clear();
	m_FileSize = std::filesystem::file_size(m_FilePath);
	m_Data.resize(m_FileSize);
    std::ifstream file(m_FilePath, std::ios::binary | std::ios::in);
    if (file.is_open() == false)
        throw std::runtime_error("Couldn't open file: " + m_FilePath + " to extract data");

    file.read(reinterpret_cast<char*>(m_Data.data()), m_FileSize);
    file.close();
}

void htn::File::ReleaseData()
{
	m_Data.clear();
}

void htn::File::SaveData(bool adding, std::string outputPath)
{
    SaveData(m_Data, adding, outputPath);
}

void htn::File::SaveData(std::vector<uint8_t>& rData, bool adding, std::string outputPath)
{
    int openMode = std::ios::binary | std::ios::out | (adding == true ? std::ios::app : std::ios::trunc);
    std::string& outpath = outputPath == "" ? m_FilePath : outputPath;

    std::ofstream file(outpath, openMode);
    if (file.is_open() == false)
        throw std::runtime_error("Couldn't open file: " + outpath + " to save");

	file.write(reinterpret_cast<char*>(rData.data()), rData.size());
    file.close();
}

void htn::File::Deflate(int flushLevel, std::vector<uint8_t>* out)
{
    z_stream strm{};
    switch (deflateInit(&strm, flushLevel))
    {
    case Z_STREAM_ERROR:
        throw std::runtime_error("Stream Invalid parameter.");
        break;
    case Z_MEM_ERROR:
        throw std::runtime_error("Insufficient memory available.");
        break;
    case Z_VERSION_ERROR:
        throw std::runtime_error("The version requested is not compatible with the library version, or the z_stream size differs from that used by the library.");
        break;
    default:
        break;
    }
    size_t dataSize = m_Data.size();
    strm.next_in = m_Data.data();
    strm.avail_in = dataSize;
    CompressionHeader header{ dataSize, CRC::CRC32(m_Data.data(), dataSize) };


    uLong deflateOutSize = deflateBound(&strm, dataSize);
    bool isOverriding = false;
    std::vector<uint8_t>* output = nullptr;
    if (out != nullptr)
    {
        if(out->size() < deflateOutSize)
            out->resize(deflateOutSize);
        output = out;
    }
    else
    {
        isOverriding = true;
        output = new std::vector<uint8_t>(deflateOutSize);
    }
    strm.next_out = output->data();
    strm.avail_out = output->size();


    if (deflate(&strm, Z_FINISH) != Z_STREAM_END)
    {
        deflateEnd(&strm);
        if(isOverriding == true)
            delete output;
        throw std::runtime_error("Deflate not ended");
    }

    size_t headerSize = sizeof(CompressionHeader);
    output->resize(strm.total_out + headerSize);
    memcpy(output->data() + (output->size() - headerSize), &header, headerSize);
    if (isOverriding == true)
    {
        m_Data.resize(output->size());
        memcpy(m_Data.data(), output->data(), output->size());
        delete output;
    }

    deflateEnd(&strm);
}

void htn::File::Inflate(std::vector<uint8_t>* out)
{
    size_t headerSize = sizeof(CompressionHeader);
    if (m_Data.size() < headerSize)
        throw std::runtime_error("Data size is smaller than header size => not supported");

    z_stream strm{};
    switch (inflateInit(&strm))
    {
    case Z_STREAM_ERROR:
        throw std::runtime_error("Stream Invalid parameter.");
        break;
    case Z_MEM_ERROR:
        throw std::runtime_error("Insufficient memory available.");
        break;
    case Z_VERSION_ERROR:
        throw std::runtime_error("The version requested is not compatible with the library version, or the z_stream size differs from that used by the library.");
        break;
    default:
        break;
    }
    size_t dataSize = m_Data.size();
    strm.next_in = m_Data.data();
    strm.avail_in = dataSize;


    CompressionHeader header{};
    memcpy(&header, m_Data.data() + (m_Data.size() - headerSize), headerSize);

    bool isOverriding = false;
    std::vector<uint8_t>* output = nullptr;
    if (out != nullptr)
    {
        if (out->size() < header.decompressedSize)
            out->resize(header.decompressedSize);
        output = out;
    }
    else
    {
        isOverriding = true;
        output = new std::vector<uint8_t>(header.decompressedSize);
    }
    strm.next_out = output->data();
    strm.avail_out = output->size();


    if (inflate(&strm, Z_FINISH) != Z_STREAM_END)
    {
        inflateEnd(&strm);
        if(isOverriding == true)
            delete output;
        throw std::runtime_error("Inflate not ended");
    }

    output->resize(strm.total_out);
    inflateEnd(&strm);

    uint32_t crc32 = CRC::CRC32(output->data(), output->size());
    if (header.crc32 != crc32)
    {
        if (isOverriding == true)
            delete output;
        throw std::runtime_error("Inflate of: " + m_FilePath + " failed. CRC32: " + std::to_string(crc32) + " instead of: " + std::to_string(header.crc32));
    }
   
    if (isOverriding == true)
    {
        m_Data.resize(output->size());
        memcpy(m_Data.data(), output->data(), output->size());
        delete output;
    }
}