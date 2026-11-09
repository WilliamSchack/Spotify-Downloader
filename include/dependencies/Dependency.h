#ifndef DEPENDENCY_H
#define DEPENDENCY_H

#include "FileUtils.h"

#include <string>

template<typename Derived>
class Dependency
{
    public:
        static bool IsInstalled()
        {
            return !GetPath().empty();
        }

        static bool IsInstalledRelative()
        {
            std::filesystem::path relativePath = FileUtils::GetRelativePath(GetRelativePath());
            return std::filesystem::exists(relativePath);
        }

        static bool DownloadFile()
        {
            return Derived::DownloadFileImpl();
        }

        static std::filesystem::path GetPath()
        {
            // Check relative path
            std::filesystem::path relativePath = FileUtils::GetRelativePath(GetRelativePath());
            if (std::filesystem::exists(relativePath))
                return relativePath;
            
            // Check system
            std::optional<std::filesystem::path> installedPath = ExternalProcess::FindInstalledPath(Derived::GetFileName());
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
    private:
        static std::string GetRelativePath()
        {
#if WIN32
            return Derived::GetFileName() + ".exe";
#else
            return Derived::GetFileName();
#endif
        }

        static std::string GetFileName()
        {
            return Derived::GetFileNameImpl();
        }
    private:
        static inline std::string _latestVersionCache = "";
};

#endif