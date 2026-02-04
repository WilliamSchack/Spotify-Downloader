#include "DownloadManager.h"

bool DownloadManager::Download(const std::string& url)
{
    EPlatform platformType = PlatformDetector::GetPlatformFromUrl(url);
    if (platformType == EPlatform::Unknown) return false;

    std::unique_ptr<IPlatform> platform = PlatformFactory::Create(platformType);
    if (platform == nullptr) return false;

    platform->Download(url);

    return true;
}