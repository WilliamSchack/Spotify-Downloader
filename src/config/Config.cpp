
#include "Config.h"

bool Config::YouTubeCookiesValid()
{
    // TODO:
    // Need to add some way of checking the po token, or checking when updating it in the settings screen
    // Also should check the age to check if age restricted videos can be accessed

    bool cookiesValid = YouTubeCookieFile::Get().CookiesValid();
    return cookiesValid && !YouTubePOToken.empty();
}