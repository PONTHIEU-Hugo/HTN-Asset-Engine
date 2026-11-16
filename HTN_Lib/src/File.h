#ifndef FILE_H
#define FILE_H

#include <stdint.h>
#include <string>
#include <vector>

#include "headers.h"


namespace htn
{
    class File
    {
    private:
        std::string m_FilePath = "";
        uint64_t m_FileSize = 0;

        std::vector<uint8_t> m_Data;

    public:
        File(std::string filePath);
        ~File() = default;

        void ExtractData();
        void ReleaseData();

        void SaveData(bool adding = false, std::string outputPath = "");
        void SaveData(std::vector<uint8_t>& rData, bool adding = false, std::string outputPath = "");

        // If out = nullptr => File's data will be override
        // flushLevel 1 = high speed / flushLevel 9 = high compression
        void Deflate(int flushLevel, std::vector<uint8_t>* out = nullptr);

        // If out = nullptr => File's data will be override
        void Inflate(std::vector<uint8_t>* out = nullptr);
    };
} // namespace htn


#endif