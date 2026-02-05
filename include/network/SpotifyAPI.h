#ifndef SPOTIFYAPI_H
#define SPOTIFYAPI_H

#include "NetworkRequest.h"
#include "TrackData.h"
#include "PlaylistData.h"

#include <vector>
#include <nlohmann/json.hpp>

class SpotifyAPI {
	public:
		SpotifyAPI();

		bool CheckConnection();
		nlohmann::json GetTrack(const std::string& id);
		nlohmann::json GetEpisode(const std::string& id);
		nlohmann::json GetPlaylist(const std::string& id);
		nlohmann::json GetPlaylistTracks(const std::string& id);
		nlohmann::json GetAlbum(const std::string& id);
		nlohmann::json GetAlbumTracks(const nlohmann::json& album);
	private:
		static inline const std::string TOKEN_URL = "https://accounts.spotify.com/api/token";
		static inline const std::string API_BASE_URL = "https://api.spotify.com/v1";

		std::string _auth;
	private:
		nlohmann::json SendRequest(const std::string& url);
		nlohmann::json GetTracks(nlohmann::json json);
		
		TrackData ParseTrack(const nlohmann::json& json);
		PlaylistData ParsePlaylist(const nlohmann::json& json);
		AlbumData ParseAlbum(const nlohmann::json& json);
		ArtistData ParseArtist(const nlohmann::json& json);
		std::vector<ArtistData> ParseArtists(const nlohmann::json& json);
};

#endif