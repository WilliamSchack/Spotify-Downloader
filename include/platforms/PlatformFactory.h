#ifndef PLATFORMFACTORY_H
#define PLATFORMFACTORY_H

#include <memory>

#include "EPlatform.h"
#include "SpotifyPlatform.h"

class PlatformFactory
{
    public:
        static std::unique_ptr<IPlatform> Create(EPlatform platform);
};

#endif