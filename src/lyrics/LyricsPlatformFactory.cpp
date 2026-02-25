#include "LyricsPlatformFactory.h"

std::unique_ptr<ILyricsPlatform> Create(const ELyricsSource& source)
{
    return nullptr;
}

std::unique_ptr<ILyricsPlatform> Create(const EPlatform& platform)
{
    return nullptr;
}