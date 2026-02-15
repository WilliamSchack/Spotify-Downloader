#include "SpotifyAPINEW.h"


QNetworkRequest SpotifyAPINew::GetRequest(const QString& endpoint, const QString& id)
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

QJsonObject SpotifyAPINew::GetPageJson(const QString& endpoint, const QString& id)
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

void SpotifyAPINew::WaitForRateLimit()
{
    std::chrono::time_point currentTime = std::chrono::system_clock::now();
    std::chrono::milliseconds msSinceLastRequest = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - _lastRequestTime);
    if (msSinceLastRequest < RATE_LIMIT_MS)
        std::this_thread::sleep_for(RATE_LIMIT_MS - msSinceLastRequest);

    _lastRequestTime = std::chrono::system_clock::now();
}

QJsonObject SpotifyAPINew::GetTrack(const QString& id)
{
    QJsonObject json = GetPageJson("track", id);
    if (json.empty()) return QJsonObject();

    return ParseTrack(json);
}

QJsonObject SpotifyAPINew::GetEpisode(const QString& id)
{
    QJsonObject json = GetPageJson("episode", id);
    if (json.empty()) return QJsonObject();

    return ParseTrack(json);
}

QJsonObject SpotifyAPINew::GetAlbum(const QString& id)
{
    QJsonObject json = GetPageJson("album", id);
    if (json.empty()) return QJsonObject();

    return ParseAlbum(json);
}

QJsonObject SpotifyAPINew::GetPlaylist(const QString& id)
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

QJsonObject SpotifyAPINew::ParseTrack(QJsonObject json)
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

QJsonArray SpotifyAPINew::ParseTracks(const QJsonArray& json)
{
    QJsonArray tracks;
    for (QJsonValue trackJsonVal : json) {
        tracks.push_back(ParseTrack(trackJsonVal.toObject()));
    }

    return tracks;
}

QJsonObject SpotifyAPINew::ParseArtist(const QJsonObject& json)
{
    QJsonObject artist;
    artist["id"] = json["uri"].toString().split(":").last();
    artist["name"] = json["profile"].toObject()["name"].toString();
    artist["url"] = ARTIST_URL + artist["id"].toString();
    artist["uri"] = json["uri"].toString();

    return artist;
}

QJsonArray SpotifyAPINew::ParseArtists(const QJsonArray& json)
{
    QJsonArray artists;
    for (QJsonValue trackJsonVal : json) {
        artists.push_back(ParseArtist(trackJsonVal.toObject()));
    }

    return artists;
}

QJsonObject SpotifyAPINew::ParseAlbum(const QJsonObject& json)
{
    QJsonObject album;
    album["id"] = json["uri"].toString().split(":").last();
    album["url"] = ALBUM_URL + album["id"].toString();
    album["name"] = json["name"].toString();

    // Cover Art
    album["images"] = QJsonArray{
        QJsonObject {
            { "url", GetLargestImageUrl(json["coverArt"].toObject()["sources"].toArray()) }
        }
    };

    // Release Date
    if (json.contains("date")) {
        QJsonObject dateJson = json["date"].toObject();
        album["release_year"] = dateJson["year"].toString();
        album["release_date"] = album["release_year"].toString();
        if (dateJson.contains("month")) album["release_date"] = album["release_date"].toString() + "-" + dateJson["month"].toString();
        if (dateJson.contains("day"))   album["release_date"] = album["release_date"].toString() + "-" + dateJson["day"].toString();
    }

    // Main Artist
    if (json.contains("artists")) {
        album["artists"] = ParseArtists(json["artists"].toObject()["items"].toArray());
    } else if (json.contains("publisher")) {
        album["artists"] = QJsonArray{
            QJsonObject {
                { "name", json["publusher"].toObject()["name"].toString() }
            }
        };
    }

    // Tracks
    QJsonObject tracksJson;
    if      (json.contains("tracks"))   tracksJson = json["tracks"].toObject();
    else if (json.contains("tracksV2")) tracksJson = json["tracksV2"].toObject();
    if (!tracksJson.empty()) {
        QJsonArray tracksJsonArray = tracksJson["items"].toArray();
        album["total_tracks"] = tracksJsonArray.size();
        QJsonArray parsedTracks = ParseTracks(tracksJsonArray);

        // Add release date
        for (int i = 0; i < parsedTracks.size(); i++) {
            QJsonObject track = parsedTracks[i].toObject();
            if (track["release_date"].toString().isEmpty()) {
                track["release_date"] = album["release_date"];
                track["release_year"] = album["release_year"];
                parsedTracks[i] = track;
            }
        }

        album["tracks"] = parsedTracks;
    }

    return album;
}

QJsonObject SpotifyAPINew::ParsePlaylist(const QJsonObject& json)
{
    QJsonObject playlistJson = json["data"].toObject()["playlistV2"].toObject();

    QJsonObject playlist;
    playlist["id"] = playlistJson["id"].toString();
    playlist["name"] = playlistJson["name"].toString();
    
    // Image
    playlist["images"] = QJsonArray{
        QJsonObject {
            { "url", GetLargestImageUrl(playlistJson["images"].toObject()["items"].toArray()[0].toObject()["sources"].toArray()) }
        }
    };

    // Owner
    QJsonObject ownerJson = playlistJson["ownerV2"].toObject()["data"].toObject();
    QJsonObject owner;
    owner["id"] = ownerJson["username"].toString();
    owner["display_name"] = ownerJson["name"].toString();
    owner["url"] = USER_URL + owner["id"].toString();
    playlist["owner"] = owner;

    // Tracks
    playlist["tracks"] = ParseTracks(playlistJson["content"].toObject()["items"].toArray());

    return playlist;
}

QString SpotifyAPINew::GetLargestImageUrl(const QJsonArray& json)
{
    QString imageUrl = "";
    unsigned int highestResolution = 0;
    for (QJsonValue coverArtDetailsVal : json) {
        QJsonObject coverArtDetails = coverArtDetailsVal.toObject();

        int resolution = coverArtDetails["width"].toInt();
        if (resolution < highestResolution)
            continue;
        
        highestResolution = resolution;
        imageUrl = coverArtDetails["url"].toString();
    }

    return imageUrl;
}