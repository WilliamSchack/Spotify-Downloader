#include "DownloadManager.h"

bool DownloadManager::Download(const std::string& url, const std::string& directory)
{
    EPlatform platformType = PlatformDetector::GetPlatformFromUrl(url);
    if (platformType == EPlatform::Unknown) return false;

    std::unique_ptr<IPlatformDownloader> platform = PlatformFactory::Create(platformType);
    if (platform == nullptr) return false;

    return platform->Download(url, directory);
}