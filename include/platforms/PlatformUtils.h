#ifndef PLATFORMUTILS_h
#define PLATFORMUTILS_h

#include "EPlatform.h"

#include <string>

class PlatformUtils
{
    public:
        static std::string GetPlatformString(EPlatform platform);
}

#endif