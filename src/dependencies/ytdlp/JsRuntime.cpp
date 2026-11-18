#include "JsRuntime.h"

std::string JsRuntime::DownloadExecutableImpl(std::filesystem::path path)
{
    // Get the download details
    std::string url = "https://bellard.org/quickjs/binary_releases/";
    std::string archiveName = "quickjs";
    std::string pathInArchive = "qjs";
    std::string latestVersion = GetLatestVersion();

    if (latestVersion.empty()) {
        return "Could not retrieve the latest version";
    }

#if defined(__APPLE__) || defined(IS_ARM_CPU)
    archiveName += "-cosmo-";
#elif _WIN32
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
    NetworkResponse response = request.DownloadToFile(archivePath);

    if (response.CurlCode != CURLE_OK || !std::filesystem::exists(archivePath)) {
        std::string curlError = curl_easy_strerror(response.CurlCode);
        Logger::Warning("Failed to download QuickJS: " + curlError);
        return "Failed to download with the network error: " + curlError;
    }

    if (!ArchiveHandler::ExtractFile(archivePath, pathInArchive, path)) {
        Logger::Warning("Failed to download QuickJS: Failed to extract");
        return "Failed to extract from the archive";
    }

#if _WIN32
    std::filesystem::path dllPath = path.parent_path() / REQUIRED_DLL_NAME;
    if (!ArchiveHandler::ExtractFile(archivePath, REQUIRED_DLL_NAME, dllPath)) {
        Logger::Warning("Failed to download QuickJS: Failed to extract the required dll");
        return "Failed to extract the required dll";
    }
#endif

    std::filesystem::remove(archivePath);
    if (!std::filesystem::exists(path)) {
        return "Failed to download for an unknown reason";
    }

    return "";
}

std::string JsRuntime::UpdateExecutableImpl()
{
    std::filesystem::path path = GetPath();
    if (path.empty()) {
        Logger::Warning("Failed to update QuickJS: Could not get the executable path");
        return "Could not get the executable path";
    }

    std::filesystem::path tempPath = Paths::GetTempDir() / "qjs-temp";
    std::string downloadError = DownloadExecutableImpl(tempPath);
    if (!downloadError.empty()) {
        Logger::Warning("Failed to update QuickJs: Download error (" + downloadError + ")");
        return "Failed to download: " + downloadError;
    }

    // Replace the current executable with the updated one
    std::filesystem::path targetPath = Paths::GetDependencyPath(GetExecutableName());
    if (std::filesystem::exists(targetPath))
        std::filesystem::remove(targetPath);

    bool fileMoved = FileUtils::MoveFile(tempPath, targetPath);
    if (!fileMoved) {
        Logger::Warning("Failed to update QuickJS: Could not move from (" + FileUtils::PathToUtf8(tempPath) + ") to (" + FileUtils::PathToUtf8(targetPath) + ")");
        return "Failed to move the executable to the target path";
    }

#if _WIN32
    // Replace the dll
    std::filesystem::path dllTempPath = tempPath.parent_path() / REQUIRED_DLL_NAME;
    std::filesystem::path dllTargetPath = targetPath.parent_path() / REQUIRED_DLL_NAME;
    if (std::filesystem::exists(dllTargetPath))
        std::filesystem::remove(dllTargetPath);

    bool dllMoved = FileUtils::MoveFile(dllTempPath, dllTargetPath);
    if (!dllMoved) {
        Logger::Warning("Failed to update QuickJS: Could not move dll from (" + FileUtils::PathToUtf8(dllTempPath) + ") to (" + FileUtils::PathToUtf8(dllTargetPath) + ")");
        return "Failed to move the dll to the target path";
    }
#endif

    return "";
}

std::string JsRuntime::GetVersionImpl()
{
    std::filesystem::path path = GetPath();
    if (path.empty())
        return "";

    ExternalProcess process(path);
    process.AddArgument("-h");

    std::string output = process.Execute().Output;

    std::smatch matches;
    if (!std::regex_search(output, matches, std::regex(R"(version\s(\d+-\d+-\d+)\s)")))
        return "";

    return matches[1];
}

std::string JsRuntime::GetLatestVersionImpl()
{
    NetworkRequest request;
	request.Url = "https://bellard.org/quickjs/binary_releases/LATEST.json";
    NetworkResponse response = request.Get();

    if (response.CurlCode != CURLE_OK) {
        Logger::Warning("Failed to get the latest version for QuickJs: " + std::string(curl_easy_strerror(response.CurlCode)));
        return "";
    }

    nlohmann::json json = nlohmann::json::parse(response.Body);
    if (json.empty()) {
        Logger::Warning("Failed to get the latest version for QuickJs: Json failed to parse (" + response.Body + ")");
        return "";
    }

    return json["version"];
}

std::string JsRuntime::GetFileNameImpl()
{
    return "qjs";
}

std::vector<GitHubRelease> JsRuntime::GetReleasesImpl(const int& resultsPerPage, const int& page)
{
    NetworkRequest request;
    request.Url = "https://bellard.org/quickjs/Changelog";
    NetworkResponse response = request.Get();
    if (response.CurlCode != CURLE_OK) {
        Logger::Warning("Failed to get QuickJs release notes: " + std::string(curl_easy_strerror(response.CurlCode)));
        return {};
    }

    std::regex regex(R"((\S+?):\n\n([\S\s]+?)\n(?=\n\d|(?![\S\s])))");
    std::vector<GitHubRelease> releases;

    for (std::sregex_iterator i = std::sregex_iterator(response.Body.begin(), response.Body.end(), regex); i != std::sregex_iterator(); i++) {
        std::smatch matches = *i;
        
        GitHubRelease release;
        release.Name = matches[1];
        release.Body = matches[2];

        releases.push_back(release);
    }

    return releases;
}