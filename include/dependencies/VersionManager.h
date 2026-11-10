#ifndef SELFUPDATER_H
#define SELFUPDATER_H

#include "GitHubAPI.h"

class VersionManager
{
    public:
        static std::string GetLatestVersion(bool ignoreCache = false);
        static bool IsUpdateAvailable(bool ignoreCache = false);
    private:
        static inline std::string _latestVersionCache = "";
};

#endif