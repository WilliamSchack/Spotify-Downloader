#ifndef DOWNLOADRESULT_H
#define DOWNLOADRESULT_H

#include <filesystem>

struct DownloadResult
{
    bool Success;
    std::filesystem::path FilePath;
};

#endif