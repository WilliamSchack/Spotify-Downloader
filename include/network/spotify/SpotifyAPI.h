// Gets spotify metadata through the mobile site
// Parses the encoded json in the bottom script tag

#ifndef SPOTIFYAPI_H
#define SPOTIFYAPI_H

#include "SpotifyAuthRetriever.h"
#include "NetworkRequest.h"
#include "HtmlParser.h"
#include "StringUtils.h"
#include "JsonUtils.h"
#include "TrackData.h"
#include "AlbumTracks.h"
#include "PlaylistTracks.h"

#include <cppcodec/base64_default_rfc4648.hpp>

#include <iostream>
#include <regex>


class SpotifyAPI
{
    public:
        // Returns all details but disc number
        static TrackData GetTrack(const std::string& id);

        static TrackData GetEpisode(const std::string& id);

        // Tracks return all details
        static AlbumTracks GetAlbum(const std::string& id);

        // Opens headless browser initially to get auth
        // Tracks return all details but release date
        static PlaylistTracks GetPlaylist(const std::string& id);
    private:
        static NetworkRequest GetRequest(const std::string& endpoint, const std::string& id);
        static nlohmann::json GetPageJson(const std::string& endpoint, const std::string& id);

        
        static TrackData ParseTrack(nlohmann::json json);
        static std::vector<TrackData> ParseTracks(const nlohmann::json& json);

        static ArtistData ParseArtist(const nlohmann::json& json);
        static std::vector<ArtistData> ParseArtists(const nlohmann::json& json);
        
        static AlbumTracks ParseAlbum(const nlohmann::json& json);
        static PlaylistTracks ParsePlaylist(const nlohmann::json& json);

        static std::string GetLargestImageUrl(const nlohmann::json& json);
    private:
        static inline const std::string USER_AGENT = "Mozilla/5.0 (Linux; Android 14) Mobile";
        static inline const unsigned int PLAYLIST_REQUEST_TRACK_LIMIT = 100; 

        // TODO: Make this thread safe
        static inline SpotifyAuth _spotifyAuth;
};

#endif