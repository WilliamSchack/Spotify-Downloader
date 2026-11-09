#ifndef YTDLP_H
#define YTDLP_H

#include "Dependency.h"
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

class Ytdlp : public Dependency<Ytdlp>
{
    friend class Dependency<Ytdlp>;

    public:
        static YtdlpResult Download(const std::string& url, const std::filesystem::path& pathNoExtension, const std::vector<std::string>& extraArgs = std::vector<std::string>(), std::function<void(float)> progressCallback = nullptr);
    private:
        static bool DownloadExecutableImpl();
        static std::string GetVersionImpl();
        static std::string GetLatestVersionImpl();
        static std::string GetFileNameImpl();

        static bool DownloadJsRuntime();
        
        static YtdlpError GetError(const std::string& errorString);
};

#endif