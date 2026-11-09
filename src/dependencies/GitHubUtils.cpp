#include "GitHubUtils.h"

std::string GitHubUtils::GetLatestTag(const std::string& repoOwner, const std::string& repoName)
{
    return GetRecentTag(repoOwner, repoName, 0);
}

std::string GitHubUtils::GetRecentTag(const std::string& repoOwner, const std::string& repoName, const int& index)
{
    NetworkRequest request;
    request.Url = "https://api.github.com/repos/" + repoOwner + "/" + repoName + "/tags";
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

    return responseJson[index]["name"];
}