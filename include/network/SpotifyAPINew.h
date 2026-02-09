// Gets spotify metadata through the mobile site
// Parses the encoded json in the bottom script tag

#ifndef SPOTIFYAPINEW_H
#define SPOTIFYAPINEW_H

#include "NetworkRequest.h"
#include "HtmlParser.h"
#include "StringUtils.h"
#include "JsonUtils.h"
#include "TrackData.h"
#include "AlbumTracks.h"

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

        static TrackData ParseTrack(nlohmann::json json);
        static std::vector<TrackData> ParseTracks(const nlohmann::json& json);

        static ArtistData ParseArtist(const nlohmann::json& json);
        static std::vector<ArtistData> ParseArtists(const nlohmann::json& json);

        static AlbumTracks ParseAlbum(const nlohmann::json& json);
};

#endif