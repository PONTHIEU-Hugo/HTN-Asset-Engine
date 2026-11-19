#include "File.h"
#include <filesystem>
#include <fstream>
#include <cstring>
#include <zlib.h>

#include "Defines.h"
#include "compression.h"
#include "crc.h"

#include "crypt.h"

namespace htn
{
    bool File::IsFileExists(std::string& path, std::string extension)
    {
        if (extension != "NONE")
        {
            size_t ext_size = extension.size();
            std::string ext = path.substr(path.size() - ext_size, ext_size);

            if (ext != extension)
                return false;
        }

        std::ifstream file(path); 
        bool result = file.good();

        return result;
    }

    File::File(std::string filePath) : mFilePath(filePath)
    {
        std::ifstream file(filePath);
        if(file.good() == false)
        {
            std::ofstream creationFile(filePath);
            mIsNew = true;
            creationFile.close();
        }
        file.close();

        size_t i = filePath.size() - 1;
        if(filePath[i] == '/' || filePath[i] == '\\')
            throw std::runtime_error("File path can't begin with / or \\");
        while(i < uint64_t_max && filePath[i] != '/' && filePath[i] != '\\')
            i--;
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

        file.seekg(beginPoint);
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

        file.seekg(beginPoint);
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
        std::ios::openmode openMode = std::ios::binary | std::ios::out | (adding == true ? std::ios::app : std::ios::trunc) | std::ios::in;
        std::string& outpath = outputPath == "" ? mFilePath : outputPath;

        std::fstream file(outpath, openMode);
        if (file.is_open() == false)
            throw std::runtime_error("Couldn't open file: " + outpath + " to save");

        file.write(reinterpret_cast<char*>(rData.data()), rData.size());
        file.close();
    }

    void File::SaveData(std::vector<uint8_t> &rData, uint64_t filePosition, std::string outputPath)
    {
        std::ios::openmode openMode = std::ios::binary | std::ios::out | std::ios::in;
        std::string& outpath = outputPath == "" ? mFilePath : outputPath;

        std::fstream file(outpath, openMode);
        if (file.is_open() == false)
            throw std::runtime_error("Couldn't open file: " + outpath + " to save");

        file.seekp(filePosition);
        file.write(reinterpret_cast<char*>(rData.data()), rData.size());
        file.close();
    }

    void File::SaveData(char *rData, uint64_t dataSize, uint64_t filePosition, std::string outputPath)
    {
        std::ios::openmode openMode = std::ios::binary | std::ios::out | std::ios::in;
        std::string& outpath = outputPath == "" ? mFilePath : outputPath;

        std::fstream file(outpath, openMode);
        if (file.is_open() == false)
            throw std::runtime_error("Couldn't open file: " + outpath + " to save");

        file.seekp(filePosition);
        file.write(rData, dataSize);
        file.close();
    }

    void File::RemoveAt(uint64_t offset, uint64_t size)
    {
        uint64_t fileSize = std::filesystem::file_size(mFilePath);
        int error = 0;
        if(fileSize < offset)
            error = 1;
        else if(fileSize < size)
            error = 2;
        else if(size + offset > fileSize)
            error = 3;

        switch (error)
        {
        case 1:
            throw std::runtime_error("File size was smaller than offset in RemoveAt");
        case 2:
            throw std::runtime_error("File size was smaller than size in RemoveAt");
        case 3:
            throw std::runtime_error("File size was smaller than size + offset in RemoveAt");
        default:
            break;
        }
            

        ExtractData(offset + size, fileSize - (offset + size));
        SaveData(GetData(), offset);
        std::filesystem::resize_file(mFilePath, fileSize - size);
    }

    void File::Encrypt(std::string key, std::string outputPath)
    {
        ExtractData();

        std::string e = Crypt::AES_Encrypt((char*)mData.data(), mData.size(), key);

        if (outputPath == "")
        {
            mData.resize(e.size());
            memcpy(mData.data(), e.data(), e.size());
            SaveData();
        }
        else
        {
            File f(outputPath + "encrypted_" + GetName());
            f.GetData().resize(e.size());

            memcpy(f.GetData().data(), e.data(), e.size());

            f.SaveData();
        }
    }

    void File::Decrypt(std::string key, std::string outputPath)
    {
        ExtractData();

        std::string d = Crypt::AES_Decrypt((char*)mData.data(), mData.size(), key);

        if (outputPath == "")
        {
            mData.resize(d.size());
            memcpy(mData.data(), d.data(), d.size());
            SaveData();
        }
        else
        {
            File f(outputPath + "decrypted_" + GetName());
            f.GetData().resize(d.size());

            memcpy(f.GetData().data(), d.data(), d.size());

            f.SaveData();
        }
    }
    
}