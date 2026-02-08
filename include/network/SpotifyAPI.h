#ifndef SPOTIFYAPI_H
#define SPOTIFYAPI_H

#include "NetworkRequest.h"
#include "TrackData.h"
#include "AlbumData.h"
#include "PlaylistTracks.h"

#include <iostream>
#include <vector>
#include <nlohmann/json.hpp>

class SpotifyAPI {
	public:
		SpotifyAPI();

		bool CheckConnection();
		TrackData GetTrack(const std::string& id);
		TrackData GetEpisode(const std::string& id);
		PlaylistTracks GetPlaylist(const std::string& id);
		AlbumData GetAlbum(const std::string& id);
	private:
		static inline const std::string TOKEN_URL = "https://accounts.spotify.com/api/token";
		static inline const std::string API_BASE_URL = "https://api.spotify.com/v1";

		std::string _auth;
	private:
		nlohmann::json SendRequest(const std::string& url);
		nlohmann::json GetTracks(nlohmann::json json);
		
		TrackData ParseTrack(const nlohmann::json& json);
		std::vector<TrackData> ParseTracks(const nlohmann::json& json);
		TrackData ParseEpisode(const nlohmann::json& json);
		PlaylistData ParsePlaylist(const nlohmann::json& json);
		AlbumData ParseAlbum(const nlohmann::json& json);
		ArtistData ParseArtist(const nlohmann::json& json);
		std::vector<ArtistData> ParseArtists(const nlohmann::json& json);
};

#endif