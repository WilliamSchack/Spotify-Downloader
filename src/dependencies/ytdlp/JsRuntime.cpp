#include "JsRuntime.h"

bool JsRuntime::IsInstalled()
{
    return !GetPath().empty();
}

bool JsRuntime::IsInstalledRelative()
{
    std::filesystem::path relativePath = FileUtils::GetRelativePath(GetRelativePath());
    return std::filesystem::exists(relativePath);
}

bool JsRuntime::Download()
{
    // If its downloaded already, dont downloading again
    std::filesystem::path relativePath = FileUtils::GetRelativePath(GetRelativePath());
    if (std::filesystem::exists(relativePath))
        return false;

    // Get the download details
    std::string url = "https://bellard.org/quickjs/binary_releases/";
    std::string archiveName = "quickjs";
    std::string pathInArchive = "qjs";

    // Get the latest version
    NetworkRequest latestVersionRequest;
	latestVersionRequest.Url = "https://bellard.org/quickjs/binary_releases/LATEST.json";
    NetworkResponse latestVersionResponse = latestVersionRequest.Get();

    nlohmann::json latestVersionJson = nlohmann::json::parse(latestVersionResponse.Body);
    std::string latestVersion = latestVersionJson["version"];

#if defined(__APPLE__) || defined(IS_ARM_CPU)
    archiveName += "-cosmo-";
#elif WIN32
    archiveName += "-win-x86_64-";
    pathInArchive += ".exe";
#else
    archiveName += "-linux-x86_64-";
#endif
    archiveName += latestVersion + ".zip";
    url += archiveName;

    // Download
    std::filesystem::path archivePath = Paths::GetTempDir() / archiveName;

    NetworkRequest request;
    request.Url = url;
    request.DownloadToFile(archivePath);

    ArchiveHandler::ExtractFile(archivePath, pathInArchive, relativePath);

    std::filesystem::remove(archivePath);

    return std::filesystem::exists(relativePath);
}

std::string JsRuntime::GetRelativePath()
{
#if WIN32
    return "qjs.exe";
#else
    return "qjs";
#endif
}

std::filesystem::path JsRuntime::GetPath()
{
    // Check relative path
    std::filesystem::path relativePath = FileUtils::GetRelativePath(GetRelativePath());
    if (std::filesystem::exists(relativePath))
        return relativePath;
    
    // Check system
    std::optional<std::filesystem::path> installedPath = ExternalProcess::FindInstalledPath("qjs");
    if (installedPath.has_value())
        return *installedPath;

    return {};
}

bool JsRuntime::UpdateAvailable()
{
    return true;
}