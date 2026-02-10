#include "SpotifyAPINew.h"

NetworkRequest SpotifyAPINew::GetRequest(const std::string& endpoint, const std::string& id)
{
    std::string url = "https://open.spotify.com/" + endpoint + "/" + id;
    
    NetworkRequest request;
    request.Url = url;
    request.SetHeader("User-Agent", USER_AGENT);
	request.SetHeader("Accept", "*/*");
	request.SetHeader("Referer", "https://open.spotify.com");
	request.SetHeader("DNT", "1");
    return request;
}

nlohmann::json SpotifyAPINew::GetPageJson(const std::string& endpoint, const std::string& id)
{
    // Get page
    NetworkRequest request = GetRequest(endpoint, id);
    NetworkResponse response = request.Get();
    std::string responseHtml = response.Body;

    // Get json
    HtmlParser parser(responseHtml);
    std::string jsonString64 = parser.Select(R"(script[id="initialState"])").GetText();
    if (jsonString64.empty()) return nlohmann::json::object();

    // Decode json from base64
    std::string jsonString = base64::decode<std::string>(jsonString64);
    nlohmann::json json = nlohmann::json::parse(jsonString);

    return json["entities"]["items"].front();
}

TrackData SpotifyAPINew::GetTrack(const std::string& id)
{
    nlohmann::json json = GetPageJson("track", id);
    if (json.empty()) return TrackData();

    return ParseTrack(json);
}

TrackData SpotifyAPINew::GetEpisode(const std::string& id)
{
    nlohmann::json json = GetPageJson("episode", id);
    if (json.empty()) return TrackData();

    return ParseTrack(json);
}

AlbumTracks SpotifyAPINew::GetAlbum(const std::string& id)
{
    nlohmann::json json = GetPageJson("album", id);
    if (json.empty()) return AlbumTracks();

    return ParseAlbum(json);
}

PlaylistTracks SpotifyAPINew::GetPlaylist(const std::string& id)
{
    if (_spotifyAuth.Authorization.empty())
    _spotifyAuth = SpotifyAuthRetriever::GetAuth(GetRequest("playlist", id).Url);
    
    if (_spotifyAuth.Authorization.empty()) {
        // Could not get auth, return first 30 tracks
        std::cout << "Failed to get spotify auth. Getting the first 30 playlist tracks..." << std::endl;
        
        nlohmann::json json = GetPageJson("playlist", id);
        if (json.empty()) return PlaylistTracks();
        
        return ParsePlaylist(json);
    }

    // Get playlist tracks in blocks of 100 until we get all the tracks
    nlohmann::json json;
    unsigned int totalTracks = 1;
    unsigned int retrievedTracks = 0;

    while (retrievedTracks < totalTracks) {
        NetworkRequest request;
        request.Url = "https://api-partner.spotify.com/pathfinder/v2/query";
        request.SetHeader("User-Agent", USER_AGENT);
        request.SetHeader("Authorization", _spotifyAuth.Authorization);
        request.SetHeader("Client-Token", _spotifyAuth.ClientToken);

        nlohmann::json postData {
            {"variables", {
                {"uri", "spotify:playlist:" + id},
                {"limit", PLAYLIST_REQUEST_TRACK_LIMIT},
                {"offset", retrievedTracks}
            }},
            {"operationName", "queryPlaylist"},
            {"extensions", {
                {"persistedQuery", {
                    {"version", 1},
                    {"sha256Hash", _spotifyAuth.PlaylistQueryHash}
                }}
            }}
        };

        NetworkResponse response = request.Post(postData);
        if (response.Body.empty()) break;

        nlohmann::json currentJson = nlohmann::json::parse(response.Body);

        if (json.empty()) {
            json = currentJson;
            totalTracks = json["data"]["playlistV2"]["content"]["totalCount"];
            continue;
        }
        
        // Add new tracks to json
        JsonUtils::ExtendArray(json["data"]["playlistV2"]["content"]["items"], currentJson["data"]["playlistV2"]["content"]["items"]);
        retrievedTracks = json["data"]["playlistV2"]["content"]["items"].size();
    }

    // Incase of any error in the loop
    if (json.empty()) return PlaylistTracks();

    return ParsePlaylist(json);
}

TrackData SpotifyAPINew::ParseTrack(nlohmann::json json)
{
    if (json.contains("track"))       json = json["track"];
    else if (json.contains("itemV2")) json = json["itemV2"]["data"];

    TrackData track;
    track.Id = StringUtils::Split(json["uri"], ":").back();
    track.Name = json["name"];
    track.Description = json.value("description", "");
    track.Explicit = json["contentRating"]["label"] == "EXPLICIT";
    track.DiscNumber = json.value("discNumber", 0);
    track.TrackNumber = json.value("trackNumber", 0);
    track.PlaylistTrackNumber = 0;
    track.SetDuration(json["duration"]["totalMilliseconds"]);
    bool isEpisode = json.contains("showOrAudiobook");

    // Artists
    if (json.contains("firstArtist")) {
        nlohmann::json artistsJson = json["firstArtist"]["items"];
        JsonUtils::ExtendArray(artistsJson, json["otherArtists"]["items"]);
        track.Artists = ParseArtists(artistsJson);
    } else {
        track.Artists = ParseArtists(json["artists"]["items"]);
    }

    // Album
    nlohmann::json albumJson;
    if      (json.contains("albumOfTrack")) albumJson = json["albumOfTrack"];
    else if (isEpisode)                     albumJson = json["showOrAudiobook"]["data"];
    if (!albumJson.empty()) {
        track.Album = ParseAlbum(albumJson).Data;
        
        if (track.Album.MainArtist.Name.empty() && track.Artists.size() > 0)
            track.Album.MainArtist = track.Artists[0];
        
        if (isEpisode)
            track.Artists = std::vector<ArtistData> { track.Album.MainArtist };

        track.ReleaseDate = track.Album.ReleaseDate;
    }

    // Release date
    if (json.contains("releaseDate")) {
        nlohmann::json dateJson = json["releaseDate"];
        track.ReleaseDate = std::to_string(dateJson["year"].get<int>());
        if (dateJson.contains("month")) track.ReleaseDate += "-" + std::to_string(dateJson["month"].get<int>());
        if (dateJson.contains("day"))   track.ReleaseDate += "-" + std::to_string(dateJson["day"].get<int>());

        if (track.Album.ReleaseDate == "") {
            track.Album.ReleaseDate = track.ReleaseDate;
            track.Album.ReleaseYear = StringUtils::Split(track.ReleaseDate, "-")[0];
        }
    }

    return track;
}

std::vector<TrackData> SpotifyAPINew::ParseTracks(const nlohmann::json& json)
{
    std::vector<TrackData> tracks;
    if (!json.is_array()) return tracks;

    for (const nlohmann::json& trackJson : json) {
        tracks.push_back(ParseTrack(trackJson));
    }

    return tracks;
}

ArtistData SpotifyAPINew::ParseArtist(const nlohmann::json& json)
{
    ArtistData artist;
    artist.Id = StringUtils::Split(json["uri"], ":").back();
    artist.Name = json["profile"]["name"];

    return artist;
}

std::vector<ArtistData> SpotifyAPINew::ParseArtists(const nlohmann::json& json)
{
    std::vector<ArtistData> artists;
    if (!json.is_array()) return artists;

    for (const nlohmann::json& artistJson : json) {
        artists.push_back(ParseArtist(artistJson));
    }

    return artists;
}

AlbumTracks SpotifyAPINew::ParseAlbum(const nlohmann::json& json)
{
    AlbumTracks albumTracks;

    AlbumData album;
    album.Id = StringUtils::Split(json["uri"], ":").back();
    album.Name = json["name"];
    album.Description = json.value("description", "");
    
    // Type
    album.Type = EAlbumType::Album;
    if (json.contains("type")) {
        if      (json["type"] == "SINGLE")      album.Type = EAlbumType::Single;
        else if (json["type"] == "COMPILATION") album.Type = EAlbumType::Compilation;
    }

    // Cover Art
    album.ImageUrl = GetLargestImageUrl(json["coverArt"]["sources"]);

    // Release Date
    if (json.contains("date")) {
        nlohmann::json dateJson = json["date"];
        album.ReleaseYear = std::to_string(dateJson["year"].get<int>());
        album.ReleaseDate = std::to_string(dateJson["year"].get<int>());
        if (dateJson.contains("month")) album.ReleaseDate += "-" + std::to_string(dateJson["month"].get<int>());
        if (dateJson.contains("day"))   album.ReleaseDate += "-" + std::to_string(dateJson["day"].get<int>());
    }

    // Main Artist
    if (json.contains("artists")) {
        album.MainArtist = ParseArtist(json["artists"]["items"][0]);
    } else if (json.contains("publisher")) {
        album.MainArtist.Name = json["publisher"]["name"];
    }

    // Tracks
    nlohmann::json tracksJson;
    if      (json.contains("tracks"))   tracksJson = json["tracks"];
    else if (json.contains("tracksV2")) tracksJson = json["tracksV2"];
    if (!tracksJson.empty()) {
        tracksJson = tracksJson["items"];
        album.TotalTracks = tracksJson.size();
        albumTracks.Tracks = ParseTracks(tracksJson);

        // Add release date
        for (TrackData& track : albumTracks.Tracks) {
            if (track.ReleaseDate == "")
                track.ReleaseDate = album.ReleaseDate;
        }
    }

    albumTracks.Data = album;

    return albumTracks;
}

PlaylistTracks SpotifyAPINew::ParsePlaylist(const nlohmann::json& json)
{
    PlaylistTracks playlistTracks;

    const nlohmann::json& playlistJson = json["data"]["playlistV2"];
    PlaylistData playlist;
    playlist.Id = playlistJson["id"];
    playlist.Name = playlistJson["name"];
    playlist.Description = playlistJson["description"];
    playlist.ImageUrl = GetLargestImageUrl(playlistJson["images"]["items"][0]["sources"]);
    playlist.TotalTracks = playlistJson["content"]["totalCount"];

    // Owner
    const nlohmann::json& ownerJson = playlistJson["ownerV2"]["data"];
    ArtistData owner;
    owner.Id = ownerJson["username"];
    owner.Name = ownerJson["name"];

    playlist.Owner = owner;

    // Tracks
    std::vector<TrackData> tracks = ParseTracks(playlistJson["content"]["items"]);

    playlistTracks.Data = playlist;
    playlistTracks.Tracks = tracks;

    return playlistTracks;
}

std::string SpotifyAPINew::GetLargestImageUrl(const nlohmann::json& json)
{
    std::string imageUrl = "";
    unsigned int highestResolution = 0;
    for (nlohmann::json coverArtDetails : json) {
        int resolution = coverArtDetails["width"];
        if (resolution < highestResolution)
            continue;
        
        highestResolution = resolution;
        imageUrl = coverArtDetails["url"];
    }

    return imageUrl;
}