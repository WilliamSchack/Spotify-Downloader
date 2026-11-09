#ifndef SELFUPDATER_H
#define SELFUPDATER_H

#include "NetworkRequest.h"
#include "JsonUtils.h"

class VersionManager
{
    public:
        static std::string GetLatestVersion(bool ignoreCache = false);
        static bool IsUpdateAvailable(bool ignoreCache = false);
    private:
        static inline const std::string GITHUB_TAGS_URL = "https://api.github.com/repos/WilliamSchack/Echolite/tags";

        static inline std::string _latestVersionCache = "";
};

#endif