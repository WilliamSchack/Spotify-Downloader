#include "GitHubUtils.h"

std::string GitHubUtils::GetLatestVersion(const std::string& repoOwner, const std::string& repoName)
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

    std::string latestTag = responseJson[0]["name"];
    latestTag = latestTag.substr(1);
}