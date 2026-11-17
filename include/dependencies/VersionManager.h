#ifndef SELFUPDATER_H
#define SELFUPDATER_H

#include "GitHubAPI.h"

class VersionManager
{
    public:
        static std::string GetLatestVersion(bool ignoreCache = false);
        static bool IsUpdateAvailable(bool ignoreCache = false);
        static std::vector<GitHubRelease> GetReleases(const int& resultsPerPage, const int& page);
    private:
        static inline std::string _latestVersionCache = "";
};

#endif