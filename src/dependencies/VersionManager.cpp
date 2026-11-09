#include "VersionManager.h"

std::string VersionManager::GetLatestVersion(bool ignoreCache)
{
    if (!ignoreCache && !_latestVersionCache.empty())
        return _latestVersionCache;

    NetworkRequest request;
    request.Url = GITHUB_TAGS_URL;
    request.SetHeader("User-Agent", "WilliamSchack/Echolite");
    request.SetHeader("Content-Type", "application/json");
    NetworkResponse response = request.Get();

    if (response.CurlCode != CURLE_OK) {
        std::cout << "Failed to get the latest version: " << curl_easy_strerror(response.CurlCode) << std::endl;
        return "";
    }

    nlohmann::json responseJson = nlohmann::json::parse(response.Body);
    if (!responseJson.is_array()) {
        std::cout << "Failed to get the latest version: " << responseJson.dump() << std::endl;
        return "";
    }

    // Get the tag and remove the leading v
    std::string latestTag = responseJson[0]["name"];
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