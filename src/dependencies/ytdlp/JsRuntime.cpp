#include "JsRuntime.h"

bool JsRuntime::DownloadExecutableImpl()
{
    // If its downloaded already, dont downloading again
    std::filesystem::path relativePath = FileUtils::GetRelativePath(GetRelativePath());
    if (std::filesystem::exists(relativePath))
        return false;

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

    ArchiveHandler::ExtractFile(archivePath, pathInArchive, relativePath);

    std::filesystem::remove(archivePath);

    return std::filesystem::exists(relativePath);
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