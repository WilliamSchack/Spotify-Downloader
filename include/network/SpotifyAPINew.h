// Translation from https://github.com/AliAkhtari78/SpotifyScraper
// Only contains things relevant to this project

#ifndef SPOTIFYAPINEW_H
#define SPOTIFYAPINEW_H

#include "NetworkRequest.h"
#include "TrackData.h"
#include "JsonUtils.h"

#include <regex>

class SpotifyAPINew
{
    public:
        static TrackData GetTrack(const std::string& id);
};

#endif