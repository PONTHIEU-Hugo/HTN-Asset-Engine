#include "Archive.h"

#include <fstream>
#include <filesystem>
#include <cstring>
#include <zlib.h>

#include "File.h"
#include "crc.h"
#include "compression.h"
#include <string>

namespace htn
{
    Archive::Archive(std::string archivePath) : mArchivePath(archivePath), mFile(archivePath)
    {
        if(mFile.IsNew() == true)
            mFile.SaveData<ArchiveHeader>(mHeader);
        else
            mFile.ExtractData<ArchiveHeader>(mHeader);
    }

    void Archive::AddFile(File &file)
    {
        file.ExtractData();
        std::vector<uint8_t>& fileData = file.GetData();
        AddFile(fileData, file.GetName());
    }
    void Archive::AddFile(std::string filePath)
    {
        File file(filePath);
        AddFile(file);
    }
    void Archive::AddFile(std::vector<uint8_t> &fileData, std::string name)
    {
        if(mHeader.firstFooterPos == 0) 
            throw std::runtime_error("File loaded as Archive isn't one or there is problem in it");

        OpenFooters();

        uint64_t archiveSize = std::filesystem::file_size(mArchivePath);
        if(archiveSize > headerSize + footerSize && IsFileExisting(name) == true)
        {
            RemoveFile(name);
            archiveSize = std::filesystem::file_size(mArchivePath);
        }

        size_t fileSize = fileData.size();
        std::string fileCompressed = Compress::GetDeflatted((char*)fileData.data(), fileSize);

        // create file footer
        CRC32 fileCRC = Crc::Get(fileData.data(), fileSize);
        uLong fileDeflattedBound = Compress::current_comp_size;
        ArchiveFileFooter lastFooter{};
        uint64_t lastFooterIndex = GetLastFooter(lastFooter);
        lastFooterIndex = lastFooterIndex == 0 ? mHeader.firstFooterPos : lastFooterIndex; // Is it first file in Archive

        ArchiveFileFooter fileFooter{
            { fileCRC, fileDeflattedBound, fileSize },
            mHeader.firstFooterPos, 
            "",
            0
        };
        memcpy(fileFooter.name, name.data(), MAX_CHAR_NAME - 1);

        // Move footers
        mFile.ExtractData(mHeader.firstFooterPos, archiveSize - mHeader.firstFooterPos);    // Extract footers
        mHeader.firstFooterPos += fileFooter.compressionInfo.compressedSize;                // Move writing spot from the size of data compressed
        mFile.SaveData(mFile.GetData(), mHeader.firstFooterPos);                            // Save existing footers
        mFile.SaveData<ArchiveHeader>(mHeader, 0);                                          // Move first footer pos in header

        // Paste file data
        mFile.SaveData(fileCompressed.data(), fileCompressed.size(), mHeader.firstFooterPos - fileFooter.compressionInfo.compressedSize);

        if(std::filesystem::file_size(mArchivePath) == headerSize + fileFooter.compressionInfo.compressedSize)
        {
            mFile.SaveData<ArchiveFileFooter>(fileFooter, headerSize + fileFooter.compressionInfo.compressedSize);
            CloseFooters();
            return;
        }

        // Update footers targetDataPos and footer chain
        ArchiveFileFooter currentFooter{};
        mFile.ExtractData<ArchiveFileFooter>(currentFooter, mHeader.firstFooterPos);
        if(currentFooter.dataPos != 0)
        {
            uint64_t currentIndex = mHeader.firstFooterPos;
            while(currentFooter.nextFooterPos != 0)
            {
                currentFooter.nextFooterPos += fileFooter.compressionInfo.compressedSize;
                mFile.SaveData<ArchiveFileFooter>(currentFooter, currentIndex);
                
                currentIndex = currentFooter.nextFooterPos;
                mFile.ExtractData<ArchiveFileFooter>(currentFooter, currentFooter.nextFooterPos);
            }
            currentFooter.nextFooterPos = archiveSize + fileFooter.compressionInfo.compressedSize;
            mFile.SaveData<ArchiveFileFooter>(currentFooter, currentIndex);
        }
        mFile.SaveData<ArchiveFileFooter>(fileFooter, archiveSize + fileFooter.compressionInfo.compressedSize);
        CloseFooters();
    }

    bool Archive::RemoveFile(std::string fileName)
    {
        OpenFooters();
        if(mHeader.firstFooterPos == headerSize) 
        {
            CloseFooters();
            return false; // No files in archive or problem in it
        }

        // Archive State => [H][A][B][C][fA][fB][fC]
        // H: header ; A,B,C: file data ; fA,fB,fC: file footer

        // Search the footer we want to delete and it's position
        ArchiveFileFooter targetFooter{};
        uint64_t targetFooterPos = GetFooter(fileName, targetFooter);
        if(targetFooter.dataPos == 0) 
        {
            CloseFooters();
            return false;    // Quit function if not in archive
        }

        // Size of the footer which we want to delete & of the file        
        uint64_t archiveSize = std::filesystem::file_size(mArchivePath);
        bool isEndFile = targetFooter.nextFooterPos == 0;
        uint64_t deletedFooterSize = isEndFile == false ? 
            targetFooter.nextFooterPos - targetFooterPos : 
            archiveSize - targetFooterPos;

        // Archive State => [H][A][B][C][fA][fB][fC]
        // Goal is to remove B
        mFile.RemoveAt(targetFooter.dataPos, targetFooter.compressionInfo.compressedSize);
        mHeader.firstFooterPos -= targetFooter.compressionInfo.compressedSize;  
        mFile.SaveData<ArchiveHeader>(mHeader);  
        targetFooterPos -= targetFooter.compressionInfo.compressedSize;

        // Archive State => [H][A][C][fA][fB][fC]
        // Goal is to remove fB
        mFile.RemoveAt(targetFooterPos, deletedFooterSize);

        if(std::filesystem::file_size(mArchivePath) == headerSize)
        {
            mHeader.firstFooterPos = headerSize;
            mFile.SaveData<ArchiveHeader>(mHeader);
            CloseFooters();
            return true;
        }

        // Archive State => [H][A][C][fA][fC]
        // Goal is to update footers dataPos & footers nextPos
        ArchiveFileFooter currentFooter{};
        mFile.ExtractData<ArchiveFileFooter>(currentFooter, mHeader.firstFooterPos);
        uint64_t currentPos = mHeader.firstFooterPos;

        while(currentFooter.nextFooterPos != 0)
        {
            currentFooter.nextFooterPos -= targetFooter.compressionInfo.compressedSize;
            
            if(currentFooter.dataPos > targetFooter.dataPos)
            {
                currentFooter.dataPos -= targetFooter.compressionInfo.compressedSize;
                currentFooter.nextFooterPos -= deletedFooterSize;
            }
            
            if(currentFooter.nextFooterPos == targetFooterPos && isEndFile == true)
                currentFooter.nextFooterPos = 0;

            mFile.SaveData<ArchiveFileFooter>(currentFooter, currentPos);
            if(currentFooter.nextFooterPos != 0)
            {
                currentPos = currentFooter.nextFooterPos;
                mFile.ExtractData<ArchiveFileFooter>(currentFooter, currentPos);
            }
        }
        if(isEndFile == false)
        {
            if(currentFooter.dataPos > targetFooter.dataPos)
                currentFooter.dataPos -= targetFooter.compressionInfo.compressedSize;
            currentFooter.nextFooterPos = 0;
            mFile.SaveData<ArchiveFileFooter>(currentFooter, currentPos);
        }
        CloseFooters();
        return true;
    }

    bool Archive::ReadFile(std::string fileName, std::vector<uint8_t> &outData)
    {
        OpenFooters();
        if(mHeader.firstFooterPos == headerSize)
        {
            CloseFooters();
            return false;
        }

        ArchiveFileFooter currentFooter{};
        mFile.ExtractData<ArchiveFileFooter>(currentFooter, mHeader.firstFooterPos);
        std::string strName(currentFooter.name);

        while(currentFooter.dataPos != 0 && currentFooter.nextFooterPos != 0 && strName != fileName)
        {
            mFile.ExtractData<ArchiveFileFooter>(currentFooter, currentFooter.nextFooterPos);
            strName = currentFooter.name;
        }
        if(currentFooter.nextFooterPos == 0 && strName != fileName)
        {
            CloseFooters();
            return false;    // Quit function if not in archive
        }

        mFile.ExtractData(currentFooter.dataPos, currentFooter.compressionInfo.compressedSize, outData);
        std::string decompressedString = Compress::GetInflatted((char*)outData.data(), currentFooter.compressionInfo.decompressedSize, currentFooter.compressionInfo.compressedSize);
        outData.clear();
        outData.resize(currentFooter.compressionInfo.decompressedSize);
        memcpy(outData.data(), decompressedString.data(), currentFooter.compressionInfo.decompressedSize);

        CloseFooters();
        return true;
    }

    std::vector<std::string> Archive::GetFileNames()
    {
        std::vector<std::string> fileNames(0);
        if(mHeader.firstFooterPos == headerSize)
        {
            return fileNames;
        }

        OpenFooters();
        ArchiveFileFooter currentFooter{};
        mFile.ExtractData<ArchiveFileFooter>(currentFooter, mHeader.firstFooterPos);

        while(currentFooter.dataPos != 0 && currentFooter.nextFooterPos != 0)
        {
            std::string currentName(currentFooter.name);
            fileNames.push_back(currentName);
            mFile.ExtractData<ArchiveFileFooter>(currentFooter, currentFooter.nextFooterPos);
        }
        std::string currentName(currentFooter.name);
        fileNames.push_back(currentName);

        CloseFooters();
        return fileNames;
    }

    bool Archive::IsFileExisting(std::string &name)
    {
        if(mHeader.firstFooterPos == headerSize) return false;

        ArchiveFileFooter currentFooter{};
        mFile.ExtractData<ArchiveFileFooter>(currentFooter, mHeader.firstFooterPos);

        // Seek if file is already in archive
        while(currentFooter.nextFooterPos != 0 && currentFooter.name != name)
            mFile.ExtractData<ArchiveFileFooter>(currentFooter, currentFooter.nextFooterPos);
        if(currentFooter.nextFooterPos == 0 && currentFooter.name != name) return false;    // Quit function if not in archive

        return true;
    }

    uint64_t Archive::GetFooter(std::string fileName, ArchiveFileFooter &footerFinded)
    {
        if(mHeader.firstFooterPos == headerSize)
            throw std::runtime_error("Archive is empty");

        ArchiveFileFooter currentFooter{};
        mFile.ExtractData<ArchiveFileFooter>(currentFooter, mHeader.firstFooterPos);
        std::string strName(currentFooter.name);
        uint64_t outIndex = mHeader.firstFooterPos;

        while(currentFooter.dataPos != 0 && currentFooter.nextFooterPos != 0 && strName != fileName)
        {
            outIndex = currentFooter.nextFooterPos;
            mFile.ExtractData<ArchiveFileFooter>(currentFooter, currentFooter.nextFooterPos);
            strName = currentFooter.name;
        }
        if(strName == fileName)
            footerFinded = currentFooter;
        return outIndex;
    }
    uint64_t Archive::GetLastFooter(ArchiveFileFooter &out)
    {
        mHeader = mFile.ExtractData<ArchiveHeader>(0);
        if(mHeader.firstFooterPos == 0)
            throw std::runtime_error("Archive is empty");

        out = mFile.ExtractData<ArchiveFileFooter>(mHeader.firstFooterPos);
        if(out.dataPos == 0)
            return 0;
        uint64_t outIndex = mHeader.firstFooterPos;
        while(out.nextFooterPos != 0)
        {
            outIndex = out.nextFooterPos;
            out = mFile.ExtractData<ArchiveFileFooter>(out.nextFooterPos);
        }
        return outIndex;
    }

    void Archive::OpenFooters()
    {
        if(mOpenDepth++ > 0 || mHeader.firstFooterPos == headerSize || mHeader.compressionInfo.compressedSize == 0)
            return;

        std::vector<uint8_t> compressedFooters;
        mFile.ExtractData(mHeader.firstFooterPos, mHeader.compressionInfo.compressedSize, compressedFooters);

        std::string decompressedStr = Compress::GetInflatted((char*)compressedFooters.data(), mHeader.compressionInfo.decompressedSize, mHeader.compressionInfo.compressedSize);

        if(Crc::Get((uint8_t*)decompressedStr.data(), decompressedStr.size()) != mHeader.compressionInfo.crc32)
            throw std::runtime_error("Footers block is corrupted (CRC mismatch)");

        mFile.SaveData(decompressedStr.data(), decompressedStr.size(), mHeader.firstFooterPos);
        std::filesystem::resize_file(mArchivePath, mHeader.firstFooterPos + decompressedStr.size());
    }
    void Archive::CloseFooters()
    {
        if(mOpenDepth == 0 || --mOpenDepth > 0)
            return;

        uint64_t decompressedSize = std::filesystem::file_size(mArchivePath) - mHeader.firstFooterPos;

        if(decompressedSize == 0)
        {
            mHeader.compressionInfo = { 0, 0, 0 };
            mFile.SaveData<ArchiveHeader>(mHeader, 0);
            return;
        }

        std::vector<uint8_t> decompressedFooters;
        mFile.ExtractData(mHeader.firstFooterPos, decompressedSize, decompressedFooters);

        std::string compressedStr = Compress::GetDeflatted((char*)decompressedFooters.data(), decompressedFooters.size());

        mFile.SaveData(compressedStr.data(), compressedStr.size(), mHeader.firstFooterPos);
        std::filesystem::resize_file(mArchivePath, mHeader.firstFooterPos + compressedStr.size());

        mHeader.compressionInfo.crc32 = Crc::Get(decompressedFooters.data(), decompressedFooters.size());
        mHeader.compressionInfo.compressedSize = compressedStr.size();
        mHeader.compressionInfo.decompressedSize = decompressedFooters.size();
        mFile.SaveData<ArchiveHeader>(mHeader, 0);
    }
}