#include "VersionManager.h"

std::string VersionManager::GetLatestVersion(bool ignoreCache)
{
    if (!ignoreCache && !_latestVersionCache.empty())
        return _latestVersionCache;

    std::string latestTag = GitHubAPI::GetLatestTag("WilliamSchack", "Echolite");
    if (latestTag.empty())
        return "";

    // Get the tag and remove the leading v
    latestTag = latestTag.substr(1);

    _latestVersionCache = latestTag;
    return latestTag;
}

bool VersionManager::IsUpdateAvailable(bool ignoreCache)
{
    std::string latestVersion = GetLatestVersion(ignoreCache);
    if (latestVersion.empty())
        return false;

    return latestVersion != APP_VERSION;
}

std::vector<GitHubRelease> VersionManager::GetReleases(const int& resultsPerPage, const int& page)
{
    return GitHubAPI::GetReleases("WilliamSchack", "Echolite", resultsPerPage, page);
}