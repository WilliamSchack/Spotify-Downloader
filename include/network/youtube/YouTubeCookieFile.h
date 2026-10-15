#ifndef YOUTUBECOOKIEFILE_H
#define YOUTUBECOOKIEFILE_H

#include "CookieFile.h"

class YouTubeCookieFile
{
    public:
        static CookieFile& Get();
    private:
        static inline const std::string FILE_NAME = "YouTubeCookies.txt";
        static inline CookieFile _cookieFile{FILE_NAME};
};

#endif