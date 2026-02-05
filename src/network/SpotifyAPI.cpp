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

TrackData SpotifyAPI::GetTrack(const std::string& id)
{
	nlohmann::json json = SendRequest(API_BASE_URL + "/tracks/" + id);
	return ParseTrack(json);
}

TrackData SpotifyAPI::GetEpisode(const std::string& id)
{
	nlohmann::json json = SendRequest(API_BASE_URL + "/episodes/" + id);
	return ParseEpisode(json);
}

PlaylistData SpotifyAPI::GetPlaylist(const std::string& id)
{
	nlohmann::json json = SendRequest(API_BASE_URL + "/playlists/" + id);
	json["tracks"]["items"] = GetTracks(json["tracks"]);

	return ParsePlaylist(json);
}

AlbumData SpotifyAPI::GetAlbum(const std::string& id)
{
	nlohmann::json json = SendRequest(API_BASE_URL + "/albums/" + id);
	return ParseAlbum(json);
}

nlohmann::json SpotifyAPI::GetTracks(nlohmann::json json)
{
	nlohmann::json tracks = json["items"];

	// Continue to get tracks if more than 100 are requested
	if (json["next"] != "") {
		// Get tracks in chunks of 100
		bool finished = false;
		while (!finished) {
			if (!json.contains("next") || json["next"].is_null()) {
				finished = true;
				break;
			}

			json = SendRequest(json["next"]);
			nlohmann::json& items = json["items"];

			for(const nlohmann::json& item : items) {
				tracks.push_back(item);
			}

			if (json["next"] == "") finished = true;
		}
	}

	return tracks;
}

TrackData SpotifyAPI::ParseTrack(const nlohmann::json& json)
{
	TrackData track;
	track.Id = json.value("id", "");
	if (json.contains("external_ids"))
		track.Isrc = json["external_ids"].value("isrc", "");
	track.Name = json.value("name", "");
	track.Explicit = json.value("explicit", false);
	track.SetDuration(json.value("duration_ms", 0));
	track.DiscNumber = json.value("disc_number", 1);
	track.TrackNumber = json.value("track_number", 1);
	track.PlaylistTrackNumber = 1;
	
	if (json.contains("album"))
		track.Album = ParseAlbum(json["album"]);

	if (json.contains("artists"))
		track.Artists = ParseArtists(json["artists"]);

	return track;
}

std::vector<TrackData> SpotifyAPI::ParseTracks(const nlohmann::json& json)
{
	std::vector<TrackData> tracks;
	for (int i = 0; i < json.size(); i++) {
		TrackData track = ParseTrack(json[i]["track"]);
		track.PlaylistTrackNumber = i + 1;

		tracks.push_back(track);
	}

	return tracks;
}

TrackData SpotifyAPI::ParseEpisode(const nlohmann::json& json)
{
	TrackData track;
	track.Id = json["id"];
	track.Name = json["name"];
	track.Description = json["description"];
	track.Explicit = json["explicit"];
	track.SetDuration(json["duration_ms"]);
	track.DiscNumber = 1;
	track.TrackNumber = 1;
	track.PlaylistTrackNumber = 1;

	const nlohmann::json& showJson = json["show"];
	AlbumData show;
	show.Id = showJson["id"];
	show.Name = showJson["name"];
	show.Description = showJson["description"];
	show.TotalTracks = showJson["total_episodes"];
	show.ImageUrl = showJson["images"][0]["url"];
	show.Type = show.TotalTracks == 1 ? EAlbumType::Single : EAlbumType::Album;

	std::vector<ArtistData> artists;
	ArtistData artist;
	artist.Name = showJson["publisher"];
	artists.push_back(artist);

	show.Artists = artists;
	track.Artists = artists;

	track.Album = show;

	return track;
}

PlaylistData SpotifyAPI::ParsePlaylist(const nlohmann::json& json)
{
	PlaylistData playlist;
	playlist.Id = json.value("id", "");
	playlist.Name = json.value("name", "");
	if (json.contains("tracks"))
		playlist.TotalTracks = json["tracks"].value("total", 1);
	if (json.contains("images"))
		playlist.ImageUrl = json["images"][0]["url"];
	playlist.Description = json.value("description", "");
	if (json.contains("owner")) {
		playlist.OwnerId = json["owner"].value("id", "");
		playlist.OwnerName = json["owner"].value("display_name", "");
	}

	playlist.Tracks = ParseTracks(json["tracks"]["items"]);

	return playlist;
}

AlbumData SpotifyAPI::ParseAlbum(const nlohmann::json& json)
{
	AlbumData album;
	album.Id = json["id"];
	album.Name = json["name"];
	album.TotalTracks = json["total_tracks"];
	album.ImageUrl = json["images"][0]["url"];
	album.ReleaseDate = json["release_date"];
	album.ReleaseDatePrecision = json["release_date_precision"];
	if      (json["album_type"] == "album")  album.Type = EAlbumType::Album;
	else if (json["album_type"] == "single")      album.Type = EAlbumType::Single;
	else if (json["album_type"] == "compilation") album.Type = EAlbumType::Compilation;

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