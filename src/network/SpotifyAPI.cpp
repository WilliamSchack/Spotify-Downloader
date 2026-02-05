#include "SpotifyAPI.h"

SpotifyAPI::SpotifyAPI() {
	std::string postData;
	postData.append("grant_type=client_credentials&");

	// Use default api keys if not set
	postData.append("client_id=" + (ClientID.isEmpty() ? SPOTIFYAPI_KEY : ClientID) + "&");
	postData.append("client_secret=" + (ClientSecret.isEmpty() ? SPOTIFYAPI_SECRET : ClientSecret));
	
	NetworkRequest request;
	request.URL = TOKEN_URL;
	request.AddHeader("Content-Type", "application/x-www-form-urlencoded");

	NetworkResponse response = request.Post(postData);
	if (response.HTTPCode != 200) {
		std::cout << "Error authorizing Spotify API (HTTP " << response.HTTPCode << ")" << std::endl;
		return;
	}

	nlohmann::json responseJson = nlohmann::json::parse(response.Body);
	_auth = responseJson["access_token"].get<std::string>();
}

bool SpotifyAPI::CheckConnection() {
	if (_auth.empty())
		return false;

	NetworkRequest request;
	request.URL = API_BASE_URL;
	NetworkResponse response = request.Get();
	
	// If anything is returned, connection is ok
	return response.CurlCode == CURLcode::CURLE_OK;
}

/*
QJsonObject SpotifyAPI::GetPlaylist(QString id) {
	QNetworkAccessManager* manager = new QNetworkAccessManager();
	QUrl url = QUrl("https://api.spotify.com/v1/playlists/" + id);

	QNetworkRequest req(url);
	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	req.setRawHeader("Authorization", "Bearer " + _auth);

	QByteArray response = Network::Get(req);
	QJsonObject json = QJsonDocument::fromJson(response).object();

	return json;
}

QJsonArray SpotifyAPI::GetPlaylistTracks(QString id) {
	QNetworkAccessManager* manager = new QNetworkAccessManager();
	QUrl url = QUrl("https://api.spotify.com/v1/playlists/" + id + "/tracks");

	QNetworkRequest req(url);
	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	req.setRawHeader("Authorization", "Bearer " + _auth);

	QByteArray response = Network::Get(req);
	QJsonObject json = QJsonDocument::fromJson(response).object();

	return GetTracks(json);
}

QJsonObject SpotifyAPI::GetAlbum(QString id) {
	QNetworkAccessManager* manager = new QNetworkAccessManager();
	QUrl url = QUrl("https://api.spotify.com/v1/albums/" + id);

	QNetworkRequest req(url);
	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	req.setRawHeader("Authorization", "Bearer " + _auth);

	QByteArray response = Network::Get(req);
	QJsonObject json = QJsonDocument::fromJson(response).object();

	return json;
}

QJsonArray SpotifyAPI::GetAlbumTracks(QJsonObject album) {
	return GetTracks(album["tracks"].toObject());
}

QJsonObject SpotifyAPI::GetTrack(QString id) {
	QNetworkAccessManager* manager = new QNetworkAccessManager();
	QUrl url = QUrl("https://api.spotify.com/v1/tracks/" + id);

	QNetworkRequest req(url);
	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	req.setRawHeader("Authorization", "Bearer " + _auth);

	QByteArray response = Network::Get(req);
	if (response == nullptr) {
		"Error Getting Track...";
		return QJsonObject();
	}

	return QJsonDocument::fromJson(response).object();
}

QJsonObject SpotifyAPI::GetEpisode(QString id) {
	QNetworkAccessManager* manager = new QNetworkAccessManager();
	QUrl url = QUrl("https://api.spotify.com/v1/episodes/" + id);

	QNetworkRequest req(url);
	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	req.setRawHeader("Authorization", "Bearer " + _auth);

	QByteArray response = Network::Get(req);
	if (response == nullptr) {
		"Error Getting Track...";
		return QJsonObject();
	}

	return QJsonDocument::fromJson(response).object();
}

QJsonArray SpotifyAPI::GetTracks(QJsonObject json) {
	QJsonArray tracks = json["items"].toArray();
	
	// Continue to get tracks if more than 100 are requested
	if (json["next"].toString() != "") {
		QNetworkRequest req(QUrl(""));
		req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
		req.setRawHeader("Authorization", "Bearer " + _auth);

		// Get tracks in chunks of 100
		bool finished = false;
		while (!finished) {
			if (json["next"].toString() == "") {
				finished = true;
				break;
			}

			req.setUrl(json["next"].toString());

			QByteArray response = Network::Get(req);
			if (response == nullptr) {
				qWarning() << "Error Getting Tracks...";
				return QJsonArray();
			}

			json = QJsonDocument::fromJson(response).object();
			QJsonArray items = json["items"].toArray();

			foreach(QJsonValue item, items) {
				tracks.append(item);
			}

			if (json["next"].toString() == "") finished = true;
		}
	}

	// Add playlist track number, the one included is the position in the album
	// Tracks will be positioned at its count in the spotify playlist, use index for track number
	for (int i = 0; i < tracks.count(); i++) {
		// Get track
		QJsonObject track = tracks[i].toObject();
		QJsonObject trackData = track["track"].toObject();

		// Add playlist track number
		trackData["playlist_track_number"] = i + 1;

		// Add data to original track
		track["track"] = trackData;
		tracks[i] = track;
	}

	return tracks;
}
*/