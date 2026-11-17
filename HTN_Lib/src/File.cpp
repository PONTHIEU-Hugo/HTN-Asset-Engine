#include "File.h"
#include <filesystem>
#include <fstream>
#include <cstring>
#include <zlib.h>

#include "compress.h"
#include "crc.h"

namespace htn
{
    htn::File::File(std::string filePath) : 
	    mFilePath(filePath)
    {
        size_t i = filePath.size() - 1;
        if(filePath[i] == '/' || filePath[i] == '\\')
            throw std::runtime_error("File path can't begin with / or \\");
        while(i >= 0 && (filePath[i] != '/' || filePath[i] != '\\'))
            i++;
        mNameBegin = i + 1;
    }

    void File::ExtractData()
    {
        mData.clear();
        mFileSize = std::filesystem::file_size(mFilePath);
        mData.resize(mFileSize);
        std::ifstream file(mFilePath, std::ios::binary | std::ios::in);
        if (file.is_open() == false)
            throw std::runtime_error("Couldn't open file: " + mFilePath + " to extract data");

        file.read(reinterpret_cast<char*>(mData.data()), mFileSize);
        file.close();
    }

    void File::ExtractData(offset_t beginPoint, size_t extractionSize)
    {
        mData.clear();
        mFileSize = std::filesystem::file_size(mFilePath);
        extractionSize = extractionSize > mFileSize ? mFileSize : extractionSize;

        mData.resize(extractionSize);
        std::ifstream file(mFilePath, std::ios::binary | std::ios::in);
        if (file.is_open() == false)
            throw std::runtime_error("Couldn't open file: " + mFilePath + " to extract data");

        file.read(reinterpret_cast<char*>(mData.data()), extractionSize);
        file.close();
    }

    void File::ExtractData(offset_t beginPoint, size_t extractionSize, std::vector<uint8_t> &output)
    {
        output.clear();
        mFileSize = std::filesystem::file_size(mFilePath);
        extractionSize = extractionSize > mFileSize ? mFileSize : extractionSize;

        output.resize(extractionSize);
        std::ifstream file(mFilePath, std::ios::binary | std::ios::in);
        if (file.is_open() == false)
            throw std::runtime_error("Couldn't open file: " + mFilePath + " to extract data");

        file.read(reinterpret_cast<char*>(output.data()), extractionSize);
        file.close();
    }

    void File::ReleaseData()
    {
        mData.clear();
    }

    void File::SaveData(bool adding, std::string outputPath)
    {
        SaveData(mData, adding, outputPath);
    }

    void File::SaveData(std::vector<uint8_t>& rData, bool adding, std::string outputPath)
    {
        std::ios::openmode openMode = std::ios::binary | std::ios::out | (adding == true ? std::ios::app : std::ios::trunc);
        std::string& outpath = outputPath == "" ? mFilePath : outputPath;

        std::ofstream file(outpath, openMode);
        if (file.is_open() == false)
            throw std::runtime_error("Couldn't open file: " + outpath + " to save");

        file.write(reinterpret_cast<char*>(rData.data()), rData.size());
        file.close();
    }

    void File::SaveData(std::vector<uint8_t> &rData, uint64_t filePosition, std::string outputPath)
    {
        std::ios::openmode openMode = std::ios::binary | std::ios::out;
        std::string& outpath = outputPath == "" ? mFilePath : outputPath;

        std::ofstream file(outpath, openMode);
        if (file.is_open() == false)
            throw std::runtime_error("Couldn't open file: " + outpath + " to save");

        file.seekp(filePosition);
        file.write(reinterpret_cast<char*>(rData.data()), rData.size());
        file.close();
    }
}