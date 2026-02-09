#ifndef SPOTIFYAPINEW_H
#define SPOTIFYAPINEW_H

#include "NetworkRequest.h"
#include "HtmlParser.h"
#include "StringUtils.h"
#include "JsonUtils.h"
#include "TrackData.h"
#include "AlbumTracks.h"

#include <nlohmann/json.hpp>
#include <cppcodec/base64_default_rfc4648.hpp>

#include <regex>

#include <iostream>


class SpotifyAPINew
{
    public:
        static TrackData GetTrack(const std::string& id);
        static AlbumTracks GetAlbum(const std::string& id);
    private:
        static NetworkRequest GetRequest(const std::string& endpoint, const std::string& id);
        static nlohmann::json GetPageJson(const std::string& endpoint, const std::string& id);
};

#endif