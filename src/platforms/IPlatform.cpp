#include "IPlatform.h"

bool IPlatform::Download(const std::string& url, const std::string& directory)
{
    std::cout << "Downloading..." << std::endl;

    ELinkType linkType = GetLinkType(url);
    std::cout << "Link type: " << (int)linkType << std::endl;

    bool directoryValid = std::filesystem::exists(directory);
    std::cout << "Directory valid: " << directoryValid << std::endl;

    return true;
}