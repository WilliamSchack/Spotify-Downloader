#ifndef SPOTIFYAPINEW_H
#define SPOTIFYAPINEW_H

#include "NetworkRequest.h"
#include "HtmlParser.h"
#include "TrackData.h"
#include "StringUtils.h"
#include "JsonUtils.h"

#include <regex>

class SpotifyAPINew
{
    public:
        static TrackData GetTrack(const std::string& id);
};

#endif