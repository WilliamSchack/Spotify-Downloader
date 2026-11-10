#ifndef JSRUNTIME_H
#define JSRUNTIME_H

#include "Dependency.h"
#include "ExternalProcess.h"
#include "NetworkRequest.h"
#include "ArchiveHandler.h"
#include "Paths.h"

#include <nlohmann/json.hpp>

#include <regex>

// Only checks for and uses quickjs right now
// May check for more system installed ones in the future but this makes it easy

class JsRuntime : public Dependency<JsRuntime>
{
    friend class Dependency<JsRuntime>;

    private:
        static bool DownloadExecutableImpl(std::filesystem::path path);
        static bool UpdateExecutableImpl();
        static std::string GetVersionImpl();
        static std::string GetLatestVersionImpl();
        static std::string GetFileNameImpl();
        static std::vector<GitHubRelease> GetReleasesImpl(const int& resultsPerPage, const int& page);
};

#endif