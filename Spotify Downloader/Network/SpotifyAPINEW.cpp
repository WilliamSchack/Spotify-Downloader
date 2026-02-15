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

QJsonObject SpotifyAPI::ParseTrack(QJsonObject json)
{
    if (json.contains("track"))       json = json["track"].toObject();
    else if (json.contains("itemV2")) json = json["itemV2"].toObject()["data"].toObject();

    bool isEpisode = json.contains("showOrAudiobook");

    QJsonObject track;
    track["id"] = json["url"].toString().split(":").last();
    track["name"] = json["name"];
    track["explicit"] = json["contentRating"].toObject()["label"].toString() == "EXPLICIT";
    track["disc_number"] = json["discNumber"].toInt(0);
    track["track_number"] = json["trackNumber"].toInt(0);
    track["duration_ms"] = json["duration"].toObject()["totalMilliseconds"].toInt();

    // Artists
    if (json.contains("firstArtist")) {
        QJsonArray artistsJson = json["firstArtist"].toObject()["items"].toArray();
        JSONUtils::Extend(artistsJson, json["otherArtists"].toObject()["items"].toArray());
        track["artists"] = ParseArtists(artistsJson);
    } else {
        track["artists"] = ParseArtists(json["artists"].toObject()["items"].toArray());
    }

    // Album
    QJsonObject albumJson;
    if      (json.contains("albumOfTrack")) albumJson = json["albumOfTrack"].toObject();
    else if (isEpisode)                     albumJson = json["showOrAudiobook"].toObject()["data"].toObject();
    if (!albumJson.empty()) {
        QJsonObject album = ParseAlbum(albumJson);
        
        if (album["artists"].toArray().size() > 0 && track["artists"].toArray().size() > 0)
            album["artists"] = QJsonArray{ track["artists"].toArray()[0] };
        
        track["album"] = album;

        if (isEpisode)
            track["artists"] = album["artists"].toArray();

        track["release_date"] = album["release_date"];
        track["release_year"] = album["release_year"];
    }

    // Release date
    if (json.contains("releaseDate")) {
        QJsonObject dateJson = json["releaseDate"].toObject();

        track["release_year"] = dateJson["year"].toString();
        track["release_date"] = track["release_year"].toString();
        if (dateJson.contains("month")) track["release_date"] = track["release_date"].toString() + "-" + dateJson["month"].toString();
        if (dateJson.contains("day"))   track["release_date"] = track["release_date"].toString() + "-" + dateJson["day"].toString();

        QJsonObject album = track["album"].toObject();
        if (album["release_date"] == "") {
            album["release_date"] = track["release_date"].toString();
            album["release_year"] = track["release_date"].toString().split("-")[0];
            track["album"] = album;
        }
    }

    return track;
}

QJsonArray SpotifyAPI::ParseTracks(const QJsonArray& json)
{
    QJsonArray tracks;
    for (QJsonValue trackJsonVal : json) {
        tracks.push_back(ParseTrack(trackJsonVal.toObject()));
    }

    return tracks;
}

QJsonObject SpotifyAPI::ParseArtist(const QJsonObject& json)
{
    QJsonObject artist;
    artist["id"] = json["uri"].toString().split(":").last();
    artist["name"] = json["profile"].toObject()["name"].toString();
    artist["url"] = ARTIST_URL + artist["id"].toString();
    artist["uri"] = json["uri"].toString();

    return artist;
}

QJsonArray SpotifyAPI::ParseArtists(const QJsonArray& json)
{
    QJsonArray artists;
    for (QJsonValue trackJsonVal : json) {
        artists.push_back(ParseArtist(trackJsonVal.toObject()));
    }

    return artists;
}

/*
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