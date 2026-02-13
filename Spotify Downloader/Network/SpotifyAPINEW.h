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
#include <chrono>
#include <thread>

// Gets spotify metadata through the mobile site
// Parses the encoded json in the bottom script tag
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

        static void WaitForRateLimit();

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
        static inline const std::chrono::milliseconds RATE_LIMIT_MS = std::chrono::milliseconds(500);

        static inline const std::string BASE_URL = "https://open.spotify.com/";
        static inline const std::string TRACK_URL = BASE_URL + "track/";
        static inline const std::string EPISODE_URL = BASE_URL + "episode/";
        static inline const std::string ARTIST_URL = BASE_URL + "artist/";
        static inline const std::string USER_URL = BASE_URL + "user/";
        static inline const std::string ALBUM_URL = BASE_URL + "album/";
        static inline const std::string PLAYLIST_URL = BASE_URL + "playlist/";

        // TODO: Make this thread safe
        static inline SpotifyAuth _spotifyAuth;
        
        static inline std::chrono::time_point _lastRequestTime = std::chrono::system_clock::now();
};

#endif