#ifndef PLATFORMDETECTOR_H
#define PLATFORMDETECTOR_H

#include "EPlatform.h"
#include "StringUtils.h"

#include <string>

class PlatformDetector {
    public:
        static EPlatform GetPlatformFromUrl(const std::string& url);
};

#endif