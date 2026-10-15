#include "YoutubeData.h"

YoutubeData::YoutubeData() : _youtube() {}

std::string YoutubeData::GetTrackDownloadError(const TrackData& track)
{
    std::filesystem::path cookieFilePath = GetCookiePath();
    bool cookiesValid = !cookieFilePath.empty();
    bool ageRestricted = AgeRestricted(track.Id);

    if (ageRestricted && !cookiesValid) {
        std::cout << "Track is age restricted with no cookies assigned" << std::endl;
        return "Video is Age Restricted. Please assign YouTube cookies in the downloading settings";
    }

    return "";
}

std::vector<std::string> YoutubeData::GetExtraDownloadArgs(const TrackData& track)
{
    std::vector<std::string> args;
    std::string extractorArgs = "youtube:player_client=default";

    std::filesystem::path cookieFilePath = GetCookiePath();
    bool usingCookies = !cookieFilePath.empty() && !Config::YouTubePOToken.empty();
    if (usingCookies) {
        extractorArgs = "youtube:player_client=web_music;po_token=web_music.gvs+" + Config::YouTubePOToken;

        args.push_back("--cookies");
        args.push_back(FileUtils::PathToUtf8(cookieFilePath));
    }

    args.push_back("--extractor-args");
    args.push_back(extractorArgs);

    return args;
}

std::string YoutubeData::UpdateUrl(const TrackData& track)
{
    std::string url = "";
    bool usingCookies = GetCookiePath().empty() && !Config::YouTubePOToken.empty();
    if (usingCookies) {
        url = "https://music.youtube.com/watch?v=";
    } else {
        url = "https://www.youtube.com/watch?v=";
    }

    url += track.Id;
    return url;
}

std::filesystem::path YoutubeData::GetCookiePath()
{
    if (!Config::YouTubeCookiesValid())
        return "";

    if (!HasPremium())
        return "";

    return YouTubeCookieFile::Get().GetPath();
}

bool YoutubeData::AgeRestricted(const std::string& trackId)
{
    return _youtube.IsAgeRestricted(trackId);
}

bool YoutubeData::HasPremium()
{
    if (_premiumCacheSet)
        return _premiumCache;

    _premiumCache = _youtube.HasPremium();
    _premiumCacheSet = true;

    return _premiumCache;
}