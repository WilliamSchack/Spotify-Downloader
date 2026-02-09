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

    return json;
}

TrackData SpotifyAPINew::GetTrack(const std::string& id)
{
    nlohmann::json json = GetPageJson("track", id);
    if (json.empty()) return TrackData();

    json = json["entities"]["items"].front();

    TrackData track = ParseTrack(json);

    std::cout << track.Id << std::endl;
    std::cout << track.Name << std::endl;
    std::cout << track.ReleaseDate << std::endl;
    std::cout << track.Explicit << std::endl;
    std::cout << track.DurationMilliseconds << std::endl;
    std::cout << track.DurationSeconds << std::endl;
    std::cout << track.DiscNumber << std::endl;
    std::cout << track.TrackNumber << std::endl;
    std::cout << track.PlaylistTrackNumber << std::endl;
    std::cout << "ALBUM:" << std::endl;
    std::cout << track.Album.Id << std::endl;
    std::cout << track.Album.Name << std::endl;
    std::cout << track.Album.TotalTracks << std::endl;
    std::cout << track.Album.ReleaseDate << std::endl;
    std::cout << (int)track.Album.Type << std::endl;
    std::cout << track.Album.MainArtist.Id << std::endl;
    std::cout << track.Album.MainArtist.Name << std::endl;
    std::cout << "ARTISTS:" << std::endl;
    for (auto test : track.Artists) {
        std::cout << test.Id << std::endl;
        std::cout << test.Name << std::endl;
    }
    
    return track;

    /*

    // Track details
    TrackData track;
    track.Id = json["id"];
    track.Name = json["name"];
    track.DiscNumber = 0;                    // Cannot be retrieved
    track.TrackNumber = json["trackNumber"]; // The track number in the album, in its disc
    track.PlaylistTrackNumber = 0;           // Playlist(s) is unknown
    track.Explicit = json["contentRating"]["label"] == "EXPLICIT";
    track.SetDuration(json["duration"]["totalMilliseconds"]);

    // Artist details
    nlohmann::json artistsJson = json["firstArtist"]["items"];
    JsonUtils::ExtendArray(artistsJson, json["otherArtists"]["items"]);

    std::vector<ArtistData> artists;
    for (nlohmann::json artistJson : artistsJson) {
        ArtistData artist;
        artist.Id = artistJson["id"];
        artist.Name = artistJson["profile"]["name"];

        artists.push_back(artist);
    }

    track.Artists = artists;

    // Album details
    nlohmann::json albumJson = json["albumOfTrack"];
    
    std::cout << albumJson << std::endl;

    AlbumData album;
    album.Id = StringUtils::Split(albumJson["uri"], ":").back();
    album.Name = albumJson["name"];
    album.TotalTracks = albumJson["tracks"]["items"].size();
    album.Type = EAlbumType::Album;
    if      (albumJson["type"] == "SINGLE")      album.Type = EAlbumType::Single;
    else if (albumJson["type"] == "COMPILATION") album.Type = EAlbumType::Compilation;
    
    unsigned int highestResolution = 0;
    for (nlohmann::json coverArtDetails : albumJson["coverArt"]["sources"]) {
        int resolution = coverArtDetails["width"];
        if (resolution < highestResolution)
        continue;
        
        highestResolution = resolution;
        album.ImageUrl = coverArtDetails["url"];
    }
    
    nlohmann::json dateJson = albumJson["date"];
    album.ReleaseYear = std::to_string(dateJson["year"].get<int>());
    track.ReleaseDate = std::to_string(dateJson["year"].get<int>());
    if (dateJson.contains("month")) track.ReleaseDate += "-" + std::to_string(dateJson["month"].get<int>());
    if (dateJson.contains("day"))   track.ReleaseDate += "-" + std::to_string(dateJson["day"].get<int>());

    album.MainArtist = artists[0];
    track.Album = album;

    return track;
    */
}

AlbumTracks SpotifyAPINew::GetAlbum(const std::string& id)
{
    nlohmann::json json = GetPageJson("album", id);
    if (json.empty()) return AlbumTracks();

    std::cout << json << std::endl;

    return AlbumTracks();

    /*
    NetworkRequest request = GetRequest("album", id);
    NetworkResponse response = request.Get();
    std::string responseHtml = response.Body;

    std::cout << responseHtml << std::endl;

    // Get meta details
    HtmlParser parser(responseHtml);
    HtmlNode descriptionNode = parser.Select(R"(meta[property="og:description"])");
    HtmlNode imageNode = parser.Select(R"(meta[property="og:image"])");
    HtmlNode mainArtistIdNode = parser.Select(R"(meta[name="music:musician"])");
    HtmlNode releaseDateNode = parser.Select(R"(meta[name="music:release_date"])");
    std::vector<HtmlNode> songUrlNodes = parser.SelectAll(R"(meta[name="music:song"])");
    std::vector<HtmlNode> songDiscNodes = parser.SelectAll(R"(meta[name="music:song:disc"])");
    std::vector<HtmlNode> songTrackNodes = parser.SelectAll(R"(meta[name="music:song:track"])");

    // Parse meta details
    std::string description = descriptionNode.GetAttribute("content");
    std::vector<std::string> descriptionSplit = StringUtils::Split(description, "\U000000B7"); // dot char
    std::string mainArtistName = descriptionSplit[0];
    std::string albumTypeString = descriptionSplit[1];
    std::string albumYear = descriptionSplit[2];
    std::string songCountString = descriptionSplit[3];
    mainArtistName = mainArtistName.substr(0, mainArtistName.size() - 1);    // Remove last whitespace
    albumTypeString = albumTypeString.substr(1, albumTypeString.size() - 2); // Remove start and end whitespace
    albumYear = albumYear.substr(1, albumYear.size() - 2);                   // Remove start and end whitespace
    songCountString = songCountString.substr(1, songCountString.size() - 7); // Remove start whitespace, and ending " songs"
    
    EAlbumType albumType = EAlbumType::Album;
    if      (albumTypeString == "single")      albumType = EAlbumType::Single;
    else if (albumTypeString == "compilation") albumType = EAlbumType::Compilation;
    
    int songCount = std::stoi(songCountString);

    std::string imageUrl = imageNode.GetAttribute("content");
    std::string mainArtistId = mainArtistIdNode.GetAttribute("content");
    std::string releaseDate = releaseDateNode.GetAttribute("content");

    // Get json details
    HtmlNode jsonNode = parser.Select(R"(script[type="application/ld+json"])");
    nlohmann::json json = nlohmann::json::parse(jsonNode.GetText());
    std::string albumName = json["name"];

    // Get songs details
    std::vector<TrackData> tracks;
    for (int i = 0; i < songUrlNodes.size(); i++) {
        TrackData track;

        track.Id = StringUtils::Split(songUrlNodes[i].GetAttribute("content"), "/").back();
        track.ReleaseDate = releaseDate;
        track.DiscNumber = std::stoi(songDiscNodes[i].GetAttribute("content"));
        track.TrackNumber = std::stoi(songTrackNodes[i].GetAttribute("content"));
        track.PlaylistTrackNumber = 1;  // Playlist(s) is unknown

        // Details
        HtmlNode trackDetailsParentNode = parser.Select(R"(div:has(> a[href="/track/)" + track.Id + R"("]))");
        HtmlNode trackNameNode = parser.Select(trackDetailsParentNode, R"(p[data-encore-id="listRowTitle"] span)");
        HtmlNode explicitNode = parser.Select(trackDetailsParentNode, R"(span[aria-label="Explicit"])");

        track.Name = trackNameNode.GetText();
        track.Explicit = explicitNode.Exists();

        // Artists
        std::vector<HtmlNode> trackArtistNodes = parser.SelectAll(trackDetailsParentNode, R"(p[data-encore-id="listRowDetails"] a)");

        std::vector<ArtistData> artists;
        for (int j = 0; j < trackArtistNodes.size(); j++) {
            ArtistData artist;
            artist.Id = StringUtils::Split(trackArtistNodes[j].GetAttribute("href"), "/").back();
            artist.Name = trackArtistNodes[j].GetText();
            
            artists.push_back(artist);
        }

        // track.album = album;
        track.Artists = artists;

        tracks.push_back(track);
    }

    return AlbumTracks();
    */
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
        track.Album.MainArtist = track.Artists[0];
        track.ReleaseDate = track.Album.ReleaseDate;

        std::cout << track.Album.MainArtist.Id << std::endl;
        std::cout << track.Album.MainArtist.Name << std::endl;
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

    nlohmann::json tracksJson;
    if      (json.contains("tracks"))   tracksJson = json["tracks"];
    else if (json.contains("tracksV2")) tracksJson = json["tracksV2"];
    if (!tracksJson.empty()) {
        tracksJson = tracksJson["items"];
        album.TotalTracks = tracksJson.size();
        albumTracks.Tracks = ParseTracks(tracksJson);
    }

    albumTracks.Data = album;

    return albumTracks;
}