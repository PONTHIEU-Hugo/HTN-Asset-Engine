#ifndef FILE_H
#define FILE_H

#include <stdint.h>
#include <string>
#include <vector>
#include <string_view>

#include "headers.h"


namespace htn
{
    class File
    {
    private:
        std::string mFilePath = "";
        size_t mNameBegin = 0;
        uint64_t mFileSize = 0;
        bool mIsNew = false;

        std::vector<uint8_t> mData;

    public:
        File(std::string filePath);
        ~File() = default;

        bool IsNew() { return mIsNew; }
        std::vector<uint8_t>& GetData() { return mData; }
        std::string GetName() { return mFilePath.substr(mNameBegin, mFilePath.size() - mNameBegin); }

        void ExtractData();
        void ExtractData(offset_t beginPoint, size_t extractionSize);
        void ExtractData(offset_t beginPoint, size_t extractionSize, std::vector<uint8_t>& output);
        template <typename T>
        T ExtractData(uint64_t beginPoint = 0);
        template <typename T>
        void ExtractData(T& outputValue, uint64_t beginPoint = 0);

        void ReleaseData();

        void SaveData(bool adding = false, std::string outputPath = "");
        void SaveData(std::vector<uint8_t>& rData, bool adding = false, std::string outputPath = "");
        void SaveData(std::vector<uint8_t>& rData, uint64_t filePosition = 0, std::string outputPath = "");
        void SaveData(char* rData, uint64_t dataSize, uint64_t filePosition = 0, std::string outputPath = "");
        template <typename T>
        void SaveData(const T& data, uint64_t filePosition = 0, std::string outputPath = "");

        void RemoveAt(uint64_t offset, uint64_t size);
    };
} // namespace htn

#include "File.inl"

#endif