#include "JsRuntime.h"

bool JsRuntime::DownloadExecutableImpl(std::filesystem::path path)
{
    // Get the download details
    std::string url = "https://bellard.org/quickjs/binary_releases/";
    std::string archiveName = "quickjs";
    std::string pathInArchive = "qjs";
    std::string latestVersion = GetLatestVersion();

    if (latestVersion.empty())
        return false;

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

    ArchiveHandler::ExtractFile(archivePath, pathInArchive, path);

    std::filesystem::remove(archivePath);

    return std::filesystem::exists(path);
}

bool JsRuntime::UpdateExecutableImpl()
{
    std::filesystem::path path = GetPath();
    if (path.empty())
        return false;

    std::filesystem::path tempPath = Paths::GetTempDir() / "qjs-temp";
    bool downloaded = DownloadExecutableImpl(tempPath);
    if (!downloaded)
        return false;

    // Replace the current executable with the updated one
    std::filesystem::path targetPath = FileUtils::GetRelativePath(GetRelativePath());
    if (std::filesystem::exists(targetPath))
        std::filesystem::remove(targetPath);

    bool fileMoved = FileUtils::MoveFile(tempPath, targetPath);
    if (!fileMoved)
        return false;

    return true;
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
        std::cout << "Failed to get the latest version: " << curl_easy_strerror(response.CurlCode) << std::endl;
        return "";
    }

    nlohmann::json json = nlohmann::json::parse(response.Body);
    if (json.empty())
        return "";

    return json["version"];
}

std::string JsRuntime::GetFileNameImpl()
{
    return "qjs";
}

std::vector<GitHubRelease> JsRuntime::GetReleases(const int& resultsPerPage, const int& page)
{
    NetworkRequest request;
    request.Url = "https://bellard.org/quickjs/Changelog";
    NetworkResponse response = request.Get();
    if (response.CurlCode != CURLE_OK) {
        std::cout << "Failed to get QuickJs release notes: " << curl_easy_strerror(response.CurlCode) << std::endl;
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