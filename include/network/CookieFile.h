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
        void Clear();
        void ClearCache();
        void CreateIfEmpty();

        bool CookiesValid(const std::string& cookies);
        bool CookiesValid();

        std::filesystem::path GetPath();
    private:
        bool Exists();
    private:
        std::string _contentsCache;
        std::filesystem::path _path;
};

#endif