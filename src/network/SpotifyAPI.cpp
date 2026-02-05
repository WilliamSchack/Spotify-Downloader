#include "SpotifyAPI.h"

SpotifyAPI::SpotifyAPI()
{
	std::string postData;
	postData.append("grant_type=client_credentials&");

	// Use default api keys if not set
	postData.append("client_id=" + std::string(SPOTIFYAPI_KEY) + "&");
	postData.append("client_secret=" + std::string(SPOTIFYAPI_SECRET));
	
	NetworkRequest request;
	request.URL = TOKEN_URL;
	request.SetHeader("Content-Type", "application/x-www-form-urlencoded");

	NetworkResponse response = request.Post(postData);
	if (response.HTTPCode != 200) {
		std::cout << "Error authorizing Spotify API (HTTP " << response.HTTPCode << ")" << std::endl;
		return;
	}

	nlohmann::json responseJson = nlohmann::json::parse(response.Body);
	_auth = responseJson["access_token"];
}

bool SpotifyAPI::CheckConnection()
{
	if (_auth.empty())
		return false;

	NetworkRequest request;
	request.URL = API_BASE_URL;
	NetworkResponse response = request.Get();
	
	// If anything is returned, connection is ok
	return response.CurlCode == CURLcode::CURLE_OK;
}

nlohmann::json SpotifyAPI::SendRequest(const std::string& url)
{
	NetworkRequest request;
	request.URL = url;
	request.SetHeader("Content-Type", "application/x-www-form-urlencoded");
	request.SetHeader("Authorization", "Bearer " + _auth);

	NetworkResponse response = request.Get();
	if (response.HTTPCode != 200) {
		std::cout << "Error from Spotify API: " << response.HTTPCode << std::endl;
		return nlohmann::json();
	}

	return nlohmann::json::parse(response.Body);
}

nlohmann::json SpotifyAPI::GetTrack(const std::string& id)
{
	return SendRequest(API_BASE_URL + "/tracks/" + id);
}

nlohmann::json SpotifyAPI::GetEpisode(const std::string& id)
{
	return SendRequest(API_BASE_URL + "/episodes/" + id);
}

nlohmann::json SpotifyAPI::GetPlaylist(const std::string& id)
{
	return SendRequest(API_BASE_URL + "/playlists/" + id);
}

nlohmann::json SpotifyAPI::GetPlaylistTracks(const std::string& id)
{
	nlohmann::json json = SendRequest(API_BASE_URL + "/playlists/" + id);
	return GetTracks(json);
}

nlohmann::json SpotifyAPI::GetAlbum(const std::string& id)
{
	return SendRequest(API_BASE_URL + "/albums/" + id);
}

nlohmann::json SpotifyAPI::GetAlbumTracks(nlohmann::json album)
{
	return GetTracks(album["tracks"]);
}

nlohmann::json SpotifyAPI::GetTracks(nlohmann::json json)
{
	nlohmann::json tracks = json["items"];
	
	// Continue to get tracks if more than 100 are requested
	if (json["next"] != "") {
		// Get tracks in chunks of 100
		bool finished = false;
		while (!finished) {
			if (json["next"] == "") {
				finished = true;
				break;
			}

			json = SendRequest(json["next"]);
			nlohmann::json items = json["items"];

			for(nlohmann::json item : items) {
				tracks.push_back(item);
			}

			if (json["next"] == "") finished = true;
		}
	}

	// Add playlist track number, the one included is the position in the album
	// Tracks will be positioned at its count in the spotify playlist, use index for track number
	for (int i = 0; i < tracks.size(); i++) {
		nlohmann::json trackData = tracks[i]["track"];
		trackData["playlist_track_number"] = i + 1;
	}

	return tracks;
}