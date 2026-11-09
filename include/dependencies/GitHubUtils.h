#ifndef GITHUBUTILS_H
#define GITHUBUTILS_H

#include "NetworkRequest.h"
#include "JsonUtils.h"

class GitHubUtils
{
    public:
        static std::string GetLatestVersion(const std::string& repoOwner, const std::string& repoName);
};

#endif