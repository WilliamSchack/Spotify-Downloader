#ifndef YTDLP_H
#define YTDLP_H

#include "Process.h"

#include <iostream>
#include <string>
#include <regex>

class Ytdlp
{
    public:
        static std::string GetVersion();

        static void Download(const std::string& url, const std::filesystem::path& pathNoExtension);
};

#endif