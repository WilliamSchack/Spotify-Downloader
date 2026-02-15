#include "SpotifyAPINEW.h"


QNetworkRequest SpotifyAPI::GetRequest(const QString& endpoint, const QString& id)
{
    WaitForRateLimit();

    QString url = BASE_URL + endpoint + "/" + id;
    
    QNetworkRequest request = QNetworkRequest(url);
    request.setRawHeader("User-Agent", USER_AGENT);
	request.setRawHeader("Accept", "*/*");
	request.setRawHeader("Referer", "https://open.spotify.com");
	request.setRawHeader("DNT", "1");
    return request;
}

QJsonObject SpotifyAPI::GetPageJson(const QString& endpoint, const QString& id)
{
    // Get page
    QNetworkRequest request = GetRequest(endpoint, id);
    QByteArray response = Network::Get(request);

    // Get json
    QRegularExpression regex(R"(<script\sid="initialState.+?>(.+?)<)");
    QStringList jsonMatches = regex.match(response).capturedTexts();

    if (jsonMatches.size() < 2)
        return QJsonObject();

    QString jsonString64 = jsonMatches[1];
    if (jsonString64.isEmpty()) return QJsonObject();

    // Decode json from base64
    QString jsonString = QByteArray::fromBase64(jsonString64.toUtf8());
    QJsonObject json = QJsonDocument::fromJson(jsonString.toUtf8()).object();

    return json["entities"].toObject()["items"].toArray().first().toObject();
}

void SpotifyAPI::WaitForRateLimit()
{
    std::chrono::time_point currentTime = std::chrono::system_clock::now();
    std::chrono::milliseconds msSinceLastRequest = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - _lastRequestTime);
    if (msSinceLastRequest < RATE_LIMIT_MS)
        std::this_thread::sleep_for(RATE_LIMIT_MS - msSinceLastRequest);

    _lastRequestTime = std::chrono::system_clock::now();
}

QJsonObject SpotifyAPI::GetTrack(const QString& id)
{
    QJsonObject json = GetPageJson("track", id);
    if (json.empty()) return QJsonObject();

    return ParseTrack(json);
}

QJsonObject SpotifyAPI::GetEpisode(const QString& id)
{
    QJsonObject json = GetPageJson("episode", id);
    if (json.empty()) return QJsonObject();

    return ParseTrack(json);
}

QJsonObject SpotifyAPI::GetAlbum(const QString& id)
{
    QJsonObject json = GetPageJson("album", id);
    if (json.empty()) return QJsonObject();

    return ParseAlbum(json);
}

QJsonObject SpotifyAPI::GetPlaylist(const QString& id)
{
    if (_spotifyAuth.Authorization.isEmpty())
    _spotifyAuth = SpotifyAuthRetriever::GetAuth(GetRequest("playlist", id).url());
    
    if (_spotifyAuth.Authorization.isEmpty()) {
        // Could not get auth, return first 30 tracks
        std::cout << "Failed to get spotify auth. Getting the first 30 playlist tracks..." << std::endl;
        
        QJsonObject json = GetPageJson("playlist", id);
        if (json.empty()) return QJsonObject();
        
        return ParsePlaylist(json);
    }

    // Get playlist tracks in blocks of 100 until we get all the tracks
    QJsonObject json;
    QJsonArray tracksJson;

    unsigned int totalTracks = 1;
    int retrievedTracks = 0;

    while (retrievedTracks < totalTracks) {
        WaitForRateLimit();

        QNetworkRequest request = QNetworkRequest(QString("https://api-partner.spotify.com/pathfinder/v2/query"));
        request.setRawHeader("User-Agent", USER_AGENT);
        request.setRawHeader("Authorization", _spotifyAuth.Authorization);
        request.setRawHeader("Client-Token", _spotifyAuth.ClientToken);

        QJsonObject postJson {
            {"variables", QJsonObject {
                {"uri", "spotify:playlist:" + id},
                {"limit", PLAYLIST_REQUEST_TRACK_LIMIT},
                {"offset", retrievedTracks}
            }},
            {"operationName", "queryPlaylist"},
            {"extensions", QJsonObject {
                {"persistedQuery", QJsonObject {
                    {"version", 1},
                    {"sha256Hash", _spotifyAuth.PlaylistQueryHash}
                }}
            }}
        };

        QByteArray postData = QJsonDocument(postJson).toJson();
        QByteArray response = Network::Post(request, postData);
        if (response.isEmpty()) break;

        QJsonObject currentJson = QJsonDocument::fromJson(response).object();

        if (tracksJson.empty()) {
            json = currentJson;
            tracksJson = json["data"].toObject()["playlistV2"].toObject()["content"].toObject()["items"].toArray();
            totalTracks = tracksJson.size();
            continue;
        }
        
        // Add new tracks to json
        JSONUtils::Extend(tracksJson, currentJson["data"].toObject()["playlistV2"].toObject()["content"].toObject()["items"].toArray());
        retrievedTracks = tracksJson.size();
    }

    // Merge tracks into main json
    QJsonObject jsonToMerge = {
        { "data", QJsonObject {
            { "playlistV2", QJsonObject {
                { "content", QJsonObject {
                    { "items", tracksJson }
                }}
            }}
        }}
    };

    JSONUtils::Merge(json, jsonToMerge);


    qDebug() << json;

    // Incase of any error in the loop
    if (json.empty()) return QJsonObject();

    return ParsePlaylist(json);
}

/*
TrackData SpotifyAPI::ParseTrack(nlohmann::json json)
{
    if (json.contains("track"))       json = json["track"];
    else if (json.contains("itemV2")) json = json["itemV2"]["data"];

    bool isEpisode = json.contains("showOrAudiobook");

    TrackData track(EPlatform::Spotify);
    track.Id = StringUtils::Split(json["uri"], ":").back();
    track.Url = (isEpisode ? TRACK_URL : EPISODE_URL) + track.Id;
    track.Name = json["name"];
    track.Description = json.value("description", "");
    track.Explicit = json["contentRating"]["label"] == "EXPLICIT";
    track.DiscNumber = json.value("discNumber", 0);
    track.TrackNumber = json.value("trackNumber", 0);
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
    nlohmann::json albumJson;
    if      (json.contains("albumOfTrack")) albumJson = json["albumOfTrack"];
    else if (isEpisode)                     albumJson = json["showOrAudiobook"]["data"];
    if (!albumJson.empty()) {
        track.Album = ParseAlbum(albumJson).Data;
        
        if (track.Album.Artists.size() > 0 && track.Artists.size() > 0)
            track.Album.Artists[0] = track.Artists[0];
        
        if (isEpisode)
            track.Artists = std::vector<ArtistData> { track.Album.Artists };

        track.ReleaseDate = track.Album.ReleaseDate;
        track.ReleaseYear = track.Album.ReleaseYear;
    }

    // Release date
    if (json.contains("releaseDate")) {
        nlohmann::json dateJson = json["releaseDate"];

        track.ReleaseYear = std::to_string(dateJson["year"].get<int>());
        track.ReleaseDate = track.ReleaseYear;
        if (dateJson.contains("month")) track.ReleaseDate += "-" + std::to_string(dateJson["month"].get<int>());
        if (dateJson.contains("day"))   track.ReleaseDate += "-" + std::to_string(dateJson["day"].get<int>());

        if (track.Album.ReleaseDate == "") {
            track.Album.ReleaseDate = track.ReleaseDate;
            track.Album.ReleaseYear = StringUtils::Split(track.ReleaseDate, "-")[0];
        }
    }

    return track;
}

std::vector<TrackData> SpotifyAPI::ParseTracks(const nlohmann::json& json)
{
    std::vector<TrackData> tracks;
    if (!json.is_array()) return tracks;

    for (const nlohmann::json& trackJson : json) {
        tracks.push_back(ParseTrack(trackJson));
    }

    return tracks;
}

ArtistData SpotifyAPI::ParseArtist(const nlohmann::json& json)
{
    ArtistData artist(EPlatform::Spotify);
    artist.Id = StringUtils::Split(json["uri"], ":").back();
    artist.Url = ARTIST_URL + artist.Id;
    artist.Name = json["profile"]["name"];

    return artist;
}

std::vector<ArtistData> SpotifyAPI::ParseArtists(const nlohmann::json& json)
{
    std::vector<ArtistData> artists;
    if (!json.is_array()) return artists;

    for (const nlohmann::json& artistJson : json) {
        artists.push_back(ParseArtist(artistJson));
    }

    return artists;
}

AlbumTracks SpotifyAPI::ParseAlbum(const nlohmann::json& json)
{
    AlbumTracks albumTracks;

    AlbumData album(EPlatform::Spotify);
    album.Id = StringUtils::Split(json["uri"], ":").back();
    album.Url = ALBUM_URL + album.Id;
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
        album.SetMainArtist(ParseArtist(json["artists"]["items"][0]));
    } else if (json.contains("publisher")) {
        album.SetMainArtist(ArtistData(EPlatform::Spotify));
        album.Artists[0].Name = json["publisher"]["name"];
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
            if (track.ReleaseDate == "") {
                track.ReleaseDate = album.ReleaseDate;
                track.ReleaseYear = album.ReleaseYear;
            }
        }
    }

    albumTracks.Data = album;

    return albumTracks;
}

PlaylistTracks SpotifyAPI::ParsePlaylist(const nlohmann::json& json)
{
    PlaylistTracks playlistTracks;

    const nlohmann::json& playlistJson = json["data"]["playlistV2"];
    PlaylistData playlist(EPlatform::Spotify);
    playlist.Id = playlistJson["id"];
    playlist.Url = PLAYLIST_URL + playlist.Id;
    playlist.Name = playlistJson["name"];
    playlist.Description = playlistJson["description"];
    playlist.ImageUrl = GetLargestImageUrl(playlistJson["images"]["items"][0]["sources"]);
    playlist.TotalTracks = playlistJson["content"]["totalCount"];

    // Owner
    const nlohmann::json& ownerJson = playlistJson["ownerV2"]["data"];
    ArtistData owner(EPlatform::Spotify);
    owner.Id = ownerJson["username"];
    owner.Url = USER_URL + owner.Id;
    owner.Name = ownerJson["name"];

    playlist.Owner = owner;

    // Tracks
    std::vector<TrackData> tracks = ParseTracks(playlistJson["content"]["items"]);

    playlistTracks.Data = playlist;
    playlistTracks.Tracks = tracks;

    return playlistTracks;
}

std::string SpotifyAPI::GetLargestImageUrl(const nlohmann::json& json)
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
*/