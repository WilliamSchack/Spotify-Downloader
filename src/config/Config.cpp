
#include "Config.h"

bool Config::YouTubeCookiesValid()
{
    // TODO: Check po token
    bool cookiesValid = YouTubeCookieFile::Get().CookiesValid();
    return cookiesValid && !YouTubePOToken.empty();
}