#ifndef PLATFORMDETECTOR_H
#define PLATFORMDETECTOR_H

#include <string>

#include "EPlatform.h"

class PlatformDetector {
    public:
        static EPlatform GetPlatformFromUrl(const std::string& url);
};

#endif