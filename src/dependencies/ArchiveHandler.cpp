#include "ArchiveHandler.h"

bool ArchiveHandler::Extract(const std::filesystem::path& archivePath, const std::filesystem::path& outputPath)
{
    return ExtractEntries(archivePath, [&outputPath](struct archive_entry* entry) {
        const char* entryName = archive_entry_pathname(entry);
        if (!entryName)
            return EEntryAction::Skip;

        const std::filesystem::path fullOutputPath = outputPath / entryName;
        archive_entry_set_pathname(entry, fullOutputPath.c_str());

        return EEntryAction::Continue;
    });
}

bool ArchiveHandler::ExtractFile(const std::filesystem::path& archivePath, const std::string& pathInArchive, const std::filesystem::path& outputPath)
{
    bool fileFound = false;
    bool success = ExtractEntries(archivePath, [&outputPath, &pathInArchive, &fileFound](struct archive_entry* entry) {
        const char* entryName = archive_entry_pathname(entry);
        if (!entryName || pathInArchive != entryName)
            return EEntryAction::Skip;

        archive_entry_set_pathname(entry, outputPath.c_str());

        fileFound = true;
        return EEntryAction::Break;
    });

    return success && fileFound;
}

// Based on https://github.com/libarchive/libarchive/wiki/Examples#user-content-A_Complete_Extractor
bool ArchiveHandler::ExtractEntries(const std::filesystem::path& archivePath, const std::function<EEntryAction(struct archive_entry*)>& entryCallback)
{
    struct archive *a;
    struct archive *ext;
    struct archive_entry *entry;
    int flags;
    int r;

    flags  = ARCHIVE_EXTRACT_TIME;
    flags |= ARCHIVE_EXTRACT_PERM;
    flags |= ARCHIVE_EXTRACT_ACL;
    flags |= ARCHIVE_EXTRACT_FFLAGS;
    flags |= ARCHIVE_EXTRACT_SECURE_NODOTDOT;
    flags |= ARCHIVE_EXTRACT_SECURE_SYMLINKS;

    a = archive_read_new();
    archive_read_support_format_all(a);
    archive_read_support_filter_all(a);

    ext = archive_write_disk_new();
    archive_write_disk_set_options(ext, flags);
    archive_write_disk_set_standard_lookup(ext);
    
    bool failed = false;
    if ((r = archive_read_open_filename(a, archivePath.c_str(), 10240)))
        failed = true;
    
    while (!failed) {
        r = archive_read_next_header(a, &entry);
        if (r == ARCHIVE_EOF)
            break;
        if (r < ARCHIVE_OK)
            fprintf(stderr, "%s\n", archive_error_string(a));
        if (r < ARCHIVE_WARN) {
            failed = true;
            break;
        }
        
        EEntryAction entryAction = entryCallback(entry);
        if (entryAction == EEntryAction::Abort) {
            failed = true;
            break;
        }
        if (entryAction == EEntryAction::Skip) {
            archive_read_data_skip(a);
            continue;
        }

        r = archive_write_header(ext, entry);
        if (r < ARCHIVE_OK)
            fprintf(stderr, "%s\n", archive_error_string(ext));
        else if (archive_entry_size(entry) > 0) {
            r = CopyData(a, ext);
            if (r < ARCHIVE_OK)
                fprintf(stderr, "%s\n", archive_error_string(ext));
            if (r < ARCHIVE_WARN) {
                failed = true;
                break;
            }
        }

        r = archive_write_finish_entry(ext);
        if (r < ARCHIVE_OK)
            fprintf(stderr, "%s\n", archive_error_string(ext));
        if (r < ARCHIVE_WARN)
            failed = true;

        if (entryAction == EEntryAction::Break)
            break;
    }

    archive_read_close(a);
    archive_read_free(a);
    archive_write_close(ext);
    archive_write_free(ext);
    
    return !failed;
}

// From https://github.com/libarchive/libarchive/wiki/Examples#user-content-A_Complete_Extractor
int ArchiveHandler::CopyData(struct archive* ar, struct archive* aw)
{
    int r;
    const void *buff;
    size_t size;
    la_int64_t offset;

    for (;;) {
        r = archive_read_data_block(ar, &buff, &size, &offset);
        if (r == ARCHIVE_EOF)
            return (ARCHIVE_OK);
        if (r < ARCHIVE_OK)
            return (r);
        
        r = archive_write_data_block(aw, buff, size, offset);
        if (r < ARCHIVE_OK) {
            fprintf(stderr, "%s\n", archive_error_string(aw));
            return (r);
        }
    }
}