#ifndef DOWNLOADRESULT_H
#define DOWNLOADRESULT_H

#include <string>
#include <filesystem>

struct DownloadResult
{
    bool Success;
    std::string FailReason;
    std::filesystem::path FilePath;
};

#endif