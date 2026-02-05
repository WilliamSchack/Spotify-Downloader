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

nlohmann::json SpotifyAPI::GetAlbumTracks(const nlohmann::json& album)
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

TrackData SpotifyAPI::ParseTrack(const nlohmann::json& json)
{
	TrackData track;
	track.Id = json["id"];
	track.Isrc = json["external_ids"]["isrc"];
	track.Name = json["name"];
	track.Explicit = json["explicit"];
	track.SetDuration(json["duration_ms"]);
	track.DiscNumber = json["disc_number"];
	track.TrackNumber = json["track_number"];
	track.PlaylistTrackNumber = 1;
	
	track.Album = ParseAlbum(json["album"]);
	track.Artists = ParseArtists(json["artists"]);

	return track;
}

PlaylistData SpotifyAPI::ParsePlaylist(const nlohmann::json& json)
{
	PlaylistData playlist;
	playlist.Id = json["id"];
	playlist.Name = json["name"];
	playlist.TotalTracks = json["tracks"]["total"];
	playlist.ImageUrl = json["images"]["url"];
	if (json.contains("description"))
		playlist.Description = json["description"];
	playlist.OwnerId = json["owner"]["id"];
	if (json["owner"].contains("display_name"))
		playlist.OwnerName = json["owner"]["display_name"];

	return playlist;
}

AlbumData SpotifyAPI::ParseAlbum(const nlohmann::json& json)
{
	AlbumData album;
	album.Id = json["id"];
	album.Name = json["name"];
	album.TotalTracks = json["total_tracks"];
	album.ImageUrl = json["images"]["url"];
	album.ReleaseDate = json["release_date"];
	album.ReleaseDatePrecision = json["release_date_precision"];
	if      (json["album_type"] == "album")  album.Type = EAlbumType::Album;
	else if (json["single"] == "album")      album.Type = EAlbumType::Single;
	else if (json["compilation"] == "album") album.Type = EAlbumType::Compilation;

	album.Artists = ParseArtists(json["artists"]);

	return album;
}

ArtistData SpotifyAPI::ParseArtist(const nlohmann::json& json)
{
	ArtistData artist;
	artist.Id = json["id"];
	artist.Name = json["name"];

	return artist;
}

std::vector<ArtistData> SpotifyAPI::ParseArtists(const nlohmann::json& json)
{
	std::vector<ArtistData> artists;
	for (nlohmann::json artistJson : json) {
		artists.push_back(ParseArtist(artistJson));
	}

	return artists;
}