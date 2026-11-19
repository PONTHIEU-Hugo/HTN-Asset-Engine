#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <string>
#include <vector>

#include "headers.h"
#include "File.h"

namespace htn
{
    class Archive
    {
    private:
        static const size_t headerSize = sizeof(ArchiveHeader);
        static const size_t footerSize = sizeof(ArchiveFileFooter);

    private:
        std::string mArchivePath = "";
        ArchiveHeader mHeader{};
        File mFile;
        int mOpenDepth = 0;

    public:
        Archive(std::string archivePath);
        ~Archive() = default;

        // Edit
        void AddFile(File& file);
        void AddFile(std::string filePath);
        void AddFile(std::vector<uint8_t>& fileData, std::string name);

        /* Search in current archive the file named fileName and remove it from the archive */
        bool RemoveFile(std::string fileName);
        /* Search in current archive the file file based on the name and remove it from the archive */
        void RemoveFile(File& file) { RemoveFile(file.GetName()); }

        // Read
        bool ReadFile(std::string fileName, std::vector<uint8_t>& outData);

        std::vector<std::string> GetFileNames();

    private:
        bool IsFileExisting(std::string& name);

        uint64_t GetFooter(std::string fileName, ArchiveFileFooter& footerFinded);
        uint64_t GetLastFooter(ArchiveFileFooter& out);

        void OpenFooters();
        void CloseFooters();
    };
}

#endif