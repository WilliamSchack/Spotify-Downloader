#ifndef COOKIEFILE_H
#define COOKIEFILE_H

#include "TemporaryPaths.h"

#include <fstream>
#include <iostream>

class CookieFile
{
    public:
        CookieFile(const std::string& fileName);

        std::string Read();
        void Overwrite(const std::string& contents);
    private:
        bool CookiesValid(const std::string& cookies);
        bool Exists();
    private:
        std::filesystem::path _path;
};

#endif