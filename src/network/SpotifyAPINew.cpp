#include "SpotifyAPINew.h"

NetworkRequest SpotifyAPINew::GetRequest(const std::string& endpoint, const std::string& id)
{
    std::string url = "https://open.spotify.com/" + endpoint + "/" + id;
    
    NetworkRequest request;
    request.Url = url;
    request.SetHeader("User-Agent", "Mozilla/5.0");
	request.SetHeader("Accept", "*/*");
	request.SetHeader("DNT", "1");
	request.SetHeader("Referer", "https://open.spotify.com");
    return request;
}

TrackData SpotifyAPINew::GetTrack(const std::string& id)
{
    NetworkRequest request = GetRequest("track", id);
    NetworkResponse response = request.Get();
    std::string responseHtml = response.Body;

    // Get meta details
    HtmlParser parser(responseHtml);
    HtmlNode titleNode = parser.Select(R"(meta[property="og:title"])");
    HtmlNode descriptionNode = parser.Select(R"(meta[property="og:description"])");
    HtmlNode imageNode = parser.Select(R"(meta[property="og:image"])");
    HtmlNode durationNode = parser.Select(R"(meta[name="music:duration"])");
    HtmlNode releaseDateNode = parser.Select(R"(meta[name="music:release_date"])");
    HtmlNode albumNode = parser.Select(R"(meta[name="music:album"])");
    HtmlNode albumNumberNode = parser.Select(R"(meta[name="music:album:track"])");
    HtmlNode artistsNamesNode = parser.Select(R"(meta[name="music:musician_description"])");
    std::vector<HtmlNode> artistsIdNodes = parser.SelectAll(R"(meta[name="music:musician"])");

    std::string title = titleNode.GetAttribute("content");
    std::string description = descriptionNode.GetAttribute("content");
    std::string imageUrl = imageNode.GetAttribute("content");
    unsigned int durationSeconds = std::stoi(durationNode.GetAttribute("content"));
    std::string releaseDate = releaseDateNode.GetAttribute("content");
    std::string albumUrl = albumNode.GetAttribute("content");
    std::string albumId = StringUtils::Split(albumUrl, "/").back();
    unsigned int albumNumber = std::stoi(albumNumberNode.GetAttribute("content"));
    std::vector<std::string> artistNames = StringUtils::Split(artistsNamesNode.GetAttribute("content"), ", ");

    // Explicit tag is only found in the album list at the bottom of the page
    std::string albumTrackSelector = R"(a[href="/track/)" + id + R"("])";
    std::vector<HtmlNode> trackNodes = parser.SelectAll(albumTrackSelector);
    HtmlNode albumTrackNode = trackNodes.back();
    HtmlNode explicitNode = parser.Select(albumTrackNode, R"(span[aria-label="Explicit"])");

    bool isExplicit = explicitNode.Exists();

    // Get all album tracks
    std::vector<HtmlNode> possibleAlbumTracksNode = parser.SelectAll("div:has(" + albumTrackSelector + ")");
    HtmlNode trackAlbumParentNode = possibleAlbumTracksNode.back();
    std::vector<HtmlNode> albumTracksNodes = parser.SelectAll(trackAlbumParentNode, "a");

    int albumTrackCount = albumTracksNodes.size();
    EAlbumType albumType = albumTrackCount == 1 ? EAlbumType::Single : EAlbumType::Album;

    // Get extra album details
    std::vector<std::string> descriptionSplit = StringUtils::Split(description, "\U000000B7"); // dot char
    std::string albumName = descriptionSplit[1];
    albumName = albumName.substr(1, albumName.size() - 2); // Remove start and end whitespace
    std::string albumYear = descriptionSplit.back();
    albumYear = albumYear.substr(1, albumName.size()); // Remove start whitespace

    // Parse Data
    std::vector<ArtistData> artists;
    for (int i = 0; i < artistNames.size(); i++) {
        ArtistData artist;
        artist.Id = artistsIdNodes[i].GetAttribute("content");
        artist.Name = artistNames[i];

        artists.push_back(artist);
    }

    std::vector<ArtistData> albumArtists;
    albumArtists.push_back(artists[0]); // Use first artist for album artists, could get them all but it wont be used

    AlbumData album;
    album.Id = albumId;
    album.Name = albumName;
    album.TotalTracks = albumTrackCount;
    album.ImageUrl = imageUrl;
    album.ReleaseYear = albumYear;
    album.Type = albumType;
    
    TrackData track;
    track.Id = id;
    track.Name = title;
    track.Description = description;
    track.ReleaseDate = releaseDate;
    track.Explicit = isExplicit;
    track.DurationSeconds = durationSeconds;
    track.DiscNumber = 1;            // Cannot be retrieved
    track.TrackNumber = albumNumber; // The track number in the album, in its disc
    track.PlaylistTrackNumber = 1;   // Playlist(s) is unknown
    track.Album = album;
    track.Artists = artists;

    return track;
}

AlbumTracks SpotifyAPINew::GetAlbum(const std::string& id)
{
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
    std::vector<HtmlNode> songIdNodes = parser.SelectAll(R"(meta[name="music:song"])");
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

    return AlbumTracks();
}