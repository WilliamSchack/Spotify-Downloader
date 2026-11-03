#ifndef ARCHIVEHANDLER_H
#define ARCHIVEHANDLER_H

#include <filesystem>
#include <functional>

#include <archive.h>
#include <archive_entry.h>

class ArchiveHandler
{
    public:
        static bool Extract(const std::filesystem::path& archivePath, const std::filesystem::path& outputPath);
        static bool ExtractFile(const std::filesystem::path& archivePath, const std::string& pathInArchive, const std::filesystem::path& outputPath);
    private:
        enum class EEntryAction
        {
            Continue,
            Break,
            Skip,
            Abort
        };
    private:
        static bool ExtractEntries(const std::filesystem::path& archivePath, const std::function<EEntryAction(struct archive_entry*)>& entryCallback);
        static int CopyData(struct archive* ar, struct archive* aw);
};

#endif