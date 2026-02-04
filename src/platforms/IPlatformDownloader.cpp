#include "IPlatformDownloader.h"

bool IPlatformDownloader::Download(const std::string& url, const std::string& directory)
{
    std::cout << "Downloading..." << std::endl;

    ELinkType linkType = GetLinkType(url);
    if (linkType == ELinkType::Unknown)
        return false;

    bool directoryValid = std::filesystem::exists(directory);

    

    return true;
}