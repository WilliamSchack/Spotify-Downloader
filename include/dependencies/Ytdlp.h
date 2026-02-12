#ifndef YTDLP_H
#define YTDLP_H

#include "Process.h"
#include "YtdlpResult.h"

#include <iostream>
#include <string>
#include <regex>

class Ytdlp
{
    public:
        static std::string GetVersion();

        static YtdlpResult Download(const std::string& url, const std::filesystem::path& pathNoExtension);
    private:
        static YtdlpError GetError(const std::string& errorString);
};

#endif