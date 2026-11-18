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
        size_t fileSize = std::filesystem::file_size(archivePath);
        if(fileSize >= headerSize)
        {
            std::vector<uint8_t> headerData(0);
            mFile.ExtractData(0, headerSize, headerData);
            memcpy(&mHeader, headerData.data(), headerSize);
        }
        else if(fileSize == 0)
        {
            mHeader = { {0, 0, 0}, headerSize };
            std::vector<uint8_t> castedHeader(headerSize);
            memcpy(castedHeader.data(), &mHeader, headerSize);
            mFile.SaveData(castedHeader, headerSize);
        }
        else
            throw std::runtime_error("File is not an archive");
    }

    void Archive::AddFile(File &file)
    {
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
        if(mHeader.firstFooterPos == 0) return; // No files in archive or problem in it

        size_t fileSize = fileData.size();
        std::string fileCompressed = Compress::GetDeflatted((char*)fileData.data(), fileSize);

        // create file footer
        CRC32 fileCRC = Crc::Get(fileData.data(), fileSize);
        uLong fileDeflattedBound = Compress::current_comp_size;
        ArchiveFileFooter lastFooter{};
        uint64_t lastFooterIndex = GetLastFooter(lastFooter);

        ArchiveFileFooter fileFooter = {
            { fileCRC, fileDeflattedBound, fileSize },
            lastFooterIndex,
            "",
            0
        };
        memcpy(fileFooter.name, name.data(), MAX_CHAR_NAME - 1);

        // Move footers
        uint64_t archiveSize = std::filesystem::file_size(mArchivePath);
        mFile.ExtractData(mHeader.firstFooterPos, archiveSize - mHeader.firstFooterPos);
        mHeader.firstFooterPos += fileFooter.compressionInfo.compressedSize;
        mFile.SaveData<ArchiveHeader>(mHeader);
        mFile.SaveData(mFile.GetData(), mHeader.firstFooterPos);

        // Paste file data
        mFile.SaveData<std::string>(fileCompressed, mHeader.firstFooterPos - fileFooter.compressionInfo.compressedSize);


        // Update footers targetDataPos and footer chain
        ArchiveFileFooter currentFooter{};
        GetFooter(mHeader.firstFooterPos, currentFooter);

        uint64_t currentIndex = mHeader.firstFooterPos;
        uint64_t deletedFooterSize = 0;
        bool footerAfterTarget = false;
        while(currentFooter.nextFooterPos != 0)
        {
            currentFooter.nextFooterPos += fileFooter.compressionInfo.compressedSize;
            mFile.SaveData<ArchiveFileFooter>(currentFooter, currentIndex);
            
            currentIndex = currentFooter.nextFooterPos;
            GetFooter(currentFooter.nextFooterPos, currentFooter);
        }
        mFile.SaveData<ArchiveFileFooter>(fileFooter, archiveSize + fileFooter.compressionInfo.compressedSize);
    }

    void Archive::RemoveFile(std::string fileName)
    {
        if(mHeader.firstFooterPos == 0) return; // No files in archive or problem in it

        ArchiveFileFooter currentFooter{};
        GetFooter(mHeader.firstFooterPos, currentFooter);
        
        // Seek for target file by looking footers
        while(currentFooter.nextFooterPos != 0 && currentFooter.name != fileName)
            GetFooter(currentFooter.nextFooterPos, currentFooter);

        if(currentFooter.nextFooterPos == 0 && currentFooter.name != fileName) return;    // Quit function if not in archive

        // State => currentFooter = footer of the file to delete
        ArchiveFileFooter toDeleteFooter = currentFooter;

        // Remove data file and move footers backward of size of data removed
        RemoveAt(toDeleteFooter.dataPos, toDeleteFooter.compressionInfo.compressedSize);
        mHeader.firstFooterPos -= toDeleteFooter.compressionInfo.compressedSize;    

        // Go on first footer
        GetFooter(mHeader.firstFooterPos, currentFooter);
        
        // Update footers targetDataPos and footer chain
        uint64_t currentIndex = mHeader.firstFooterPos;
        uint64_t deletedFooterSize = 0;
        bool footerAfterTarget = false;
        while(currentFooter.nextFooterPos != 0)
        {
            if(footerAfterTarget == false && currentFooter.name == fileName)
            {
                footerAfterTarget = true;
                deletedFooterSize = toDeleteFooter.compressionInfo.compressedSize + (toDeleteFooter.nextFooterPos - currentIndex);
            }
            else if(footerAfterTarget == false)
            {
                currentFooter.dataPos -= toDeleteFooter.compressionInfo.compressedSize;
                currentFooter.nextFooterPos -= toDeleteFooter.compressionInfo.compressedSize;
                mFile.SaveData<ArchiveFileFooter>(currentFooter, currentIndex);
            }
            else if(footerAfterTarget == true)
            {
                currentFooter.dataPos -= toDeleteFooter.compressionInfo.compressedSize;
                currentFooter.nextFooterPos -= deletedFooterSize;
                mFile.SaveData<ArchiveFileFooter>(currentFooter, currentIndex);
            }
            currentIndex = currentFooter.nextFooterPos;
            GetFooter(currentFooter.nextFooterPos, currentFooter);
        }
        mFile.SaveData<ArchiveHeader>(mHeader);

        std::filesystem::resize_file(mArchivePath, std::filesystem::file_size(mArchivePath) - deletedFooterSize);
    }

    void Archive::ReadFile(std::string fileName, std::vector<uint8_t> &outData)
    {
        if(mHeader.firstFooterPos == 0)
            throw std::runtime_error("Archive is empty");

        ArchiveFileFooter currentFooter{};
        GetFooter(mHeader.firstFooterPos, currentFooter);

        while(currentFooter.nextFooterPos != 0 && currentFooter.name != fileName)
            GetFooter(currentFooter.nextFooterPos, currentFooter);
        if(currentFooter.nextFooterPos == 0 && currentFooter.name != fileName) return;    // Quit function if not in archive

        outData.resize(currentFooter.compressionInfo.decompressedSize);
        mFile.ExtractData(currentFooter.dataPos, currentFooter.compressionInfo.decompressedSize, outData);
        Compress::GetInflatted((char*)outData.data(), currentFooter.compressionInfo.decompressedSize, currentFooter.compressionInfo.compressedSize);
    }

    void Archive::RemoveAt(uint64_t offset, uint64_t size)
    {
        uint64_t archiveFileSize = std::filesystem::file_size(mArchivePath);
        if(archiveFileSize < offset || archiveFileSize < size || size + offset > archiveFileSize)
            throw std::runtime_error("Remove size was bigger than archive size minimal size or simply bigger than it's size");

        mFile.ExtractData(offset + size, archiveFileSize - (offset + size));
        mFile.SaveData(mFile.GetData(), offset);
        std::filesystem::resize_file(mArchivePath, archiveFileSize - size);
    }
    
    void Archive::GetFooter(uint64_t offset, ArchiveFileFooter &out)
    {
        uint64_t archiveFileSize = std::filesystem::file_size(mArchivePath);
        if(archiveFileSize < offset || offset + footerSize > archiveFileSize)
            throw std::runtime_error("Footer doesn't fit in file at offset: " + std::to_string(offset) + ". Current offset mean the footer overflow archive size");

        mFile.ExtractData(offset, footerSize);
        memcpy(&out, mFile.GetData().data(), footerSize);
    }
    uint64_t Archive::GetLastFooter(ArchiveFileFooter &out)
    {
        mHeader = mFile.ExtractData<ArchiveHeader>(0);
        if(mHeader.firstFooterPos == 0)
            throw std::runtime_error("Archive is empty");

        out = mFile.ExtractData<ArchiveFileFooter>(mHeader.firstFooterPos);
        uint64_t outIndex = mHeader.firstFooterPos;
        while(out.nextFooterPos != 0)
        {
            outIndex = out.nextFooterPos;
            out = mFile.ExtractData<ArchiveFileFooter>(out.nextFooterPos);
        }
        return outIndex;
    }
}