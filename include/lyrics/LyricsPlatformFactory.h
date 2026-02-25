#ifndef LYRICSPLATFORMFACTORY_H
#define LYRICSPLATFORMFACTORY_H

#include "ILyricsPlatform.h"
#include "ELyricsSource.h"
#include "EPlatform.h"

#include <memory>

class LyricsPlatformFactory
{
    public:
        static std::unique_ptr<ILyricsPlatform> Create(const ELyricsSource& source);
        static std::unique_ptr<ILyricsPlatform> Create(const EPlatform& platform);
};

#endif