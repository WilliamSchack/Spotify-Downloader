#ifndef YTDLPRESULT_H
#define YTDLPRESULT_H

#include "YtdlpError.h"

#include <filesystem>

struct YtdlpResult
{
    std::filesystem::path Path = "";
    YtdlpError Error;
};

#endif