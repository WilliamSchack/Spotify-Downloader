#include "YouTubeCookieFile.h"

std::string YouTubeCookieFile::GetCookieString()
{
    return _cookieFile.Read();
}