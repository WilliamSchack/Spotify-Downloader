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
        bool CookiesValid(const std::string& cookies);
        bool CookiesValid();
    private:
        bool Exists();
    private:
        std::string _contentsCache;
        std::filesystem::path _path;
};

#endif