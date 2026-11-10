#ifndef GITHUBUTILS_H
#define GITHUBUTILS_H

#include "NetworkRequest.h"
#include "JsonUtils.h"

#include <nlohmann/json.hpp>

class GitHubAPI
{
    public:
        static std::string GetLatestTag(const std::string& repoOwner, const std::string& repoName);
        static std::string GetRecentTag(const std::string& repoOwner, const std::string& repoName, const int& index);
};

#endif