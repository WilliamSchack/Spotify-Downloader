#include "SpotifyAPINew.h"

NetworkRequest SpotifyAPINew::GetRequest(const std::string& endpoint, const std::string& id)
{
    std::string url = "https://open.spotify.com/" + endpoint + "/" + id;
    
    NetworkRequest request;
    request.Url = url;
    request.SetHeader("User-Agent", "Mozilla/5.0"); // For some reason setting a generic user agent gets the mobile page? Its exactly what we want but a bit weird
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

    return json["entities"]["items"];
}

TrackData SpotifyAPINew::GetTrack(const std::string& id)
{
    nlohmann::json json = GetPageJson("track", id);
    if (json.empty()) return TrackData();

    return ParseTrack(json);
}

AlbumTracks SpotifyAPINew::GetAlbum(const std::string& id)
{
    nlohmann::json json = GetPageJson("album", id);
    if (json.empty()) return AlbumTracks();

    return ParseAlbum(json);
}

TrackData SpotifyAPINew::ParseTrack(nlohmann::json json)
{
    if (json.contains("track"))       json = json["track"];
    else if (json.contains("itemV2")) json = json["itemV2"]["data"];

    TrackData track;
    track.Id = json["id"];
    track.Name = json["name"];
    track.Explicit = json["contentRating"]["label"] == "EXPLICIT";
    track.DiscNumber = json.value("discNumber", 0);
    track.TrackNumber = json["trackNumber"];
    track.PlaylistTrackNumber = 0;
    track.SetDuration(json["duration"]["totalMilliseconds"]);

    // Artists
    if (json.contains("firstArtist")) {
        nlohmann::json artistsJson = json["firstArtist"]["items"];
        JsonUtils::ExtendArray(artistsJson, json["otherArtists"]["items"]);
        track.Artists = ParseArtists(artistsJson);
    } else {
        track.Artists = ParseArtists(json["artists"]["items"]);
    }

    // Album
    if (json.contains("albumOfTrack")) {
        track.Album = ParseAlbum(json["albumOfTrack"]).Data;
        if (track.Album.MainArtist.Id != "")
            track.Album.MainArtist = track.Artists[0];

        track.ReleaseDate = track.Album.ReleaseDate;
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
    
    // Type
    album.Type = EAlbumType::Album;
    if (json.contains("type")) {
        if      (json["type"] == "SINGLE")      album.Type = EAlbumType::Single;
        else if (json["type"] == "COMPILATION") album.Type = EAlbumType::Compilation;
    }

    // Cover Art
    unsigned int highestResolution = 0;
    for (nlohmann::json coverArtDetails : json["coverArt"]["sources"]) {
        int resolution = coverArtDetails["width"];
        if (resolution < highestResolution)
            continue;
        
        highestResolution = resolution;
        album.ImageUrl = coverArtDetails["url"];
    }

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