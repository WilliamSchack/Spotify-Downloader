#ifndef GITHUBAPI_H
#define GITHUBAPI_H

#include "GitHubRelease.h"
#include "NetworkRequest.h"
#include "JsonUtils.h"

#include <vector>

#include <nlohmann/json.hpp>

class GitHubAPI
{
    public:
        static std::string GetLatestTag(const std::string& repoOwner, const std::string& repoName);
        static std::string GetRecentTag(const std::string& repoOwner, const std::string& repoName, const int& index);
        static std::vector<GitHubRelease> GetReleases(const std::string& repoOwner, const std::string& repoName, const int& resultsPerPage = 30, const int& page = 1);
    private:
        static NetworkRequest GetRequest(const std::string& endpoint);
};

#endif