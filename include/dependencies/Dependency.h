#ifndef DEPENDENCY_H
#define DEPENDENCY_H

#include "Paths.h"
#include "ExternalProcess.h"
#include "GitHubAPI.h"

#include <string>
#include <optional>

// Required by inheritors:
// friend class Dependency<Derived>
// static std::string DownloadExecutableImpl(std::filesystem::path path); // Returns the error if any, returns an empty string otherwise
// static std::string UpdateExecutableImpl();                             // Returns the error if any, returns an empty string otherwise
// static std::string GetVersionImpl();
// static std::string GetLatestVersionImpl();
// static std::string GetFileNameImpl();
// static std::vector<GitHubRelease> GetReleasesImpl(const int& resultsPerPage, const int& page)

template<typename Derived>
class Dependency
{
    public:
        static bool IsInstalled()
        {
            return !GetPath().empty();
        }

        static bool IsManaged()
        {
            std::filesystem::path relativePath = Paths::GetDependencyPath(GetExecutableName());
            return std::filesystem::exists(relativePath);
        }

        // Returns the error if any, returns an empty string otherwise
        static std::string DownloadExecutable()
        {
            // If its downloaded already, dont download again
            std::filesystem::path relativePath = Paths::GetDependencyPath(GetExecutableName());
            if (std::filesystem::exists(relativePath))
                return "Dependency already exists";

            return Derived::DownloadExecutableImpl(relativePath);
        }

        // Returns the error if any, returns an empty string otherwise
        static std::string UpdateExecutable()
        {
            // Dont update if installed on the system
            if (!IsManaged())
                return "This dependency is managed by the system";

            return Derived::UpdateExecutableImpl();
        }

        static std::filesystem::path GetPath()
        {
            // Check relative path
            std::filesystem::path relativePath = Paths::GetDependencyPath(GetExecutableName());
            if (std::filesystem::exists(relativePath))
                return relativePath;
            
            // Check system
            std::optional<std::filesystem::path> installedPath = ExternalProcess::FindInstalledPath(GetFileName());
            if (installedPath.has_value())
                return *installedPath;

            return {};
        }

        static std::string GetVersion()
        {
            return Derived::GetVersionImpl();
        }

        static std::string GetLatestVersion(bool ignoreCache = false)
        {
            if (!ignoreCache && !_latestVersionCache.empty())
                return _latestVersionCache;

            _latestVersionCache = Derived::GetLatestVersionImpl();
            return _latestVersionCache;
        }

        static bool IsUpdateAvailable(bool ignoreCache = false)
        {
            std::string latestVersion = GetLatestVersion(ignoreCache);
            if (latestVersion.empty())
                return false;

            return latestVersion != GetVersion();
        }

        static std::vector<GitHubRelease> GetReleases(const int& resultsPerPage = 30, const int& page = 1)
        {
            return Derived::GetReleasesImpl(resultsPerPage, page);
        }
    protected:
        static std::string GetExecutableName()
        {
#if _WIN32
            return GetFileName() + ".exe";
#else
            return GetFileName();
#endif
        }
    private:
        static std::string GetFileName()
        {
            return Derived::GetFileNameImpl();
        }
    private:
        static inline std::string _latestVersionCache = "";
};

#endif