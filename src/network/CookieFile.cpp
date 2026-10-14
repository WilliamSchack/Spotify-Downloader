#include "CookieFile.h"

CookieFile::CookieFile(const std::string& fileName)
{
    std::string fileNameValid = FileUtils::ValidateFileName(fileName);
    if (StringUtils::EndsWith(fileNameValid, ".txt"))
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
    // should validate cookies

    std::ofstream file(_path);
    file << contents;
    file.close();
}

bool CookieFile::Exists()
{
    return std::filesystem::exists(_path);
}