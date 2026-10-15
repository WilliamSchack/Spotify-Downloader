#include "YoutubeData.h"

YoutubeData::YoutubeData() : _youtube() {}

std::filesystem::path YoutubeData::GetCookiePath() const
{
    return YouTubeCookieFile::Get().GetPath();
}