#ifndef SPOTIFYAPI_H
#define SPOTIFYAPI_H

#include "SpotifyAuthRetriever.h"
#include "Utilities/JSONUtils.h"
#include "Network/Network.h"

#include <chrono>
#include <thread>

// Gets spotify metadata through the mobile site
// Parses the encoded json in the bottom script tag
class SpotifyAPI
{
    public:
        // Returns all details but disc number
        static QJsonObject GetTrack(const QString& id);

        static QJsonObject GetEpisode(const QString& id);

        // Tracks return all details
        static QJsonObject GetAlbum(const QString& id);

        // Opens headless browser initially to get auth
        // Tracks return all details but release date
        static QJsonObject GetPlaylist(const QString& id);
    private:
        static QNetworkRequest GetRequest(const QString& endpoint, const QString& id);
        static QJsonObject GetPageJson(const QString& endpoint, const QString& id);

        static void WaitForRateLimit();

        static TrackData ParseTrack(nlohmann::json json);
        static std::vector<TrackData> ParseTracks(const nlohmann::json& json);

        static ArtistData ParseArtist(const nlohmann::json& json);
        static std::vector<ArtistData> ParseArtists(const nlohmann::json& json);
        
        static AlbumTracks ParseAlbum(const nlohmann::json& json);
        static PlaylistTracks ParsePlaylist(const nlohmann::json& json);

        static std::string GetLargestImageUrl(const nlohmann::json& json);
    private:
        static inline const QByteArray USER_AGENT = "Mozilla/5.0 (Linux; Android 14) Mobile";
        static inline const int PLAYLIST_REQUEST_TRACK_LIMIT = 100; 
        static inline const std::chrono::milliseconds RATE_LIMIT_MS = std::chrono::milliseconds(500);

        static inline const QString BASE_URL = "https://open.spotify.com/";
        static inline const QString TRACK_URL = BASE_URL + "track/";
        static inline const QString EPISODE_URL = BASE_URL + "episode/";
        static inline const QString ARTIST_URL = BASE_URL + "artist/";
        static inline const QString USER_URL = BASE_URL + "user/";
        static inline const QString ALBUM_URL = BASE_URL + "album/";
        static inline const QString PLAYLIST_URL = BASE_URL + "playlist/";

        // TODO: Make this thread safe
        static inline SpotifyAuth _spotifyAuth;
        
        static inline std::chrono::time_point _lastRequestTime = std::chrono::system_clock::now();
};

#endif