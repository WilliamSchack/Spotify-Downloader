#include "CookieFile.h"

CookieFile::CookieFile(const std::string& fileName)
{
    std::string fileNameValid = FileUtils::ValidateFileName(fileName);
    if (!StringUtils::EndsWith(fileNameValid, ".txt"))
        fileNameValid += ".txt";

    _path = TemporaryPaths::GetTempDir() / fileNameValid;
}

std::string CookieFile::Read()
{
    if (!Exists())
        return "";

    std::ifstream file(_path);

    if (!file.is_open()) {
        std::cout << "Could not open cookie file at " << _path << std::endl;
        return "";
    }

    // Read the file
    std::string content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));

    file.close();
    return content;
}

void CookieFile::Overwrite(const std::string& contents)
{
    if (!CookiesValid(contents))
        return;

    std::ofstream file(_path);
    file << contents;
    file.close();
}

bool CookieFile::CookiesValid(const std::string& cookies)
{
    // Must be netscape cookies
    std::string firstLine = StringUtils::Split(cookies, "\n")[0];
    if (firstLine != "# HTTP Cookie File" && firstLine != "# Netscape HTTP Cookie File")
        return false;

    return true;
}

bool CookieFile::Exists()
{
    return std::filesystem::exists(_path);
}