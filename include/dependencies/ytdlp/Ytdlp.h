#ifndef YTDLP_H
#define YTDLP_H

#include "JsRuntime.h"
#include "Ffmpeg.h"
#include "ExternalProcess.h"
#include "NetworkRequest.h"
#include "FileUtils.h"
#include "MathUtils.h"
#include "YtdlpResult.h"
#include "Config.h"

#include <iostream>
#include <string>
#include <regex>

class Ytdlp
{
    public:
        static bool IsInstalled();
        static bool IsInstalledRelative();
        static bool Download();
        static std::filesystem::path GetPath();

        static std::string GetVersion();

        static YtdlpResult Download(const std::string& url, const std::filesystem::path& pathNoExtension, const std::vector<std::string>& extraArgs = std::vector<std::string>(), std::function<void(float)> progressCallback = nullptr);
    private:
        static bool DownloadJsRuntime();

        static std::string GetRelativePath();
        
        static YtdlpError GetError(const std::string& errorString);
};

#endif