#include "File.h"
#include <filesystem>
#include <fstream>

template <typename T>
inline T htn::File::ExtractData(uint64_t beginPoint)
{
    T out{};
    size_t typeSize = sizeof(T);
    mFileSize = std::filesystem::file_size(mFilePath);
    if(mFileSize < typeSize)
        throw std::runtime_error("Extraction failed: File size smaller than extraction type size");

    std::vector<uint8_t> data(typeSize);
    std::ifstream file(mFilePath, std::ios::binary | std::ios::in);
    if (file.is_open() == false)
        throw std::runtime_error("Couldn't open file: " + mFilePath + " to extract data");

    file.read(reinterpret_cast<char*>(data.data()), typeSize);
    file.close();

    memcpy(&out, data.data(), typeSize);
    return out;
}

template <typename T>
inline void htn::File::SaveData(const T &data, uint64_t filePosition, std::string outputPath)
{
        std::ios::openmode openMode = std::ios::binary | std::ios::out;
        std::string& outpath = outputPath == "" ? mFilePath : outputPath;

        std::ofstream file(outpath, openMode);
        if (file.is_open() == false)
            throw std::runtime_error("Couldn't open file: " + outpath + " to save");

        size_t typeSize = sizeof(T);
        std::vector<uint8_t> castedData(typeSize);
        memcpy(castedData.data(), &data, typeSize);

        file.seekp(filePosition);
        file.write(reinterpret_cast<char*>(castedData.data()), castedData.size());
        file.close();
}