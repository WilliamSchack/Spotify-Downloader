#include "GitHubAPI.h"

NetworkRequest GitHubAPI::GetRequest(const std::string& endpoint)
{
    NetworkRequest request;
    request.Url = "https://api.github.com/" + endpoint;
    request.SetHeader("User-Agent", "WilliamSchack/Echolite");
    request.SetHeader("Content-Type", "application/json");

    return request;
}

std::string GitHubAPI::GetLatestTag(const std::string& repoOwner, const std::string& repoName)
{
    return GetRecentTag(repoOwner, repoName, 0);
}

std::string GitHubAPI::GetRecentTag(const std::string& repoOwner, const std::string& repoName, const int& index)
{
    NetworkRequest request = GetRequest("repos/" + repoOwner + "/" + repoName + "/tags");
    NetworkResponse response = request.Get();

    if (response.CurlCode != CURLE_OK) {
        std::cout << "Failed to get the latest version for " << repoOwner << "/" << repoName << ": " << curl_easy_strerror(response.CurlCode) << std::endl;
        return "";
    }

    nlohmann::json responseJson = nlohmann::json::parse(response.Body);
    if (!responseJson.is_array()) {
        std::cout << "Failed to get the latest version for " << repoOwner << "/" << repoName << ": " << responseJson.dump() << std::endl;
        return "";
    }

    return responseJson[index]["name"];
}

std::vector<GitHubRelease> GitHubAPI::GetReleases(const std::string& repoOwner, const std::string& repoName, const int& resultsPerPage, const int& page)
{
    NetworkRequest request = GetRequest("repos/" + repoOwner + "/" + repoName + "/releases");
    NetworkResponse response = request.Get();

    if (response.CurlCode != CURLE_OK) {
        std::cout << "Failed to get releases: " << curl_easy_strerror(response.CurlCode) << std::endl;
        return {};
    }

    nlohmann::json responseJson = nlohmann::json::parse(response.Body);
    if (!responseJson.is_array()) {
        std::cout << "Failed to get releases: " << responseJson.dump() << std::endl;
        return {};
    }

    std::vector<GitHubRelease> releases;
    for (nlohmann::json releaseJson : responseJson) {
        GitHubRelease release;
        release.Name = releaseJson["name"];
        release.Body = releaseJson["body"];

        releases.push_back(release);
    }

    return releases;
}