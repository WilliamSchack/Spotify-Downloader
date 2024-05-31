#include "SpotifyAPI.h"

SpotifyAPI::SpotifyAPI() {
	QNetworkAccessManager* manager = new QNetworkAccessManager();
	QUrl url = QUrl("https://accounts.spotify.com/api/token");
	
	QByteArray postData;
	postData.append("grant_type=client_credentials&");
	postData.append("client_id=" + CLIENT_ID + "&");
	postData.append("client_secret=" + CLIENT_SECRET);
	
	QNetworkRequest req(url);
	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

	QByteArray response = Network::Post(req, postData);
	if (response == nullptr) {
		qDebug() << "Error Authorizing...";
		return;
	}

	QJsonObject json = QJsonDocument::fromJson(response).object();
	_auth = json["access_token"].toString().toLatin1();
}

bool SpotifyAPI::CheckConnection() {
	if (_auth.isNull()) return false;

	QUrl url = QUrl("https://api.spotify.com/v1");
	return Network::Ping(url);
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

QJsonArray SpotifyAPI::GetTracks(QJsonObject json) {
	if (json["next"].toString() == "") return json["items"].toArray();
	
	QNetworkRequest req(QUrl(""));
	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	req.setRawHeader("Authorization", "Bearer " + _auth);

	bool finished = false;
	QJsonArray tracks = json["items"].toArray();
	while (!finished) {
		if (json["next"].toString() == "") {
			finished = true;
			break;
		}

		req.setUrl(json["next"].toString());

		QByteArray response = Network::Get(req);
		if (response == nullptr) {
			qDebug() << "Error Getting Tracks...";
			return QJsonArray();
		}

		json = QJsonDocument::fromJson(response).object();
		QJsonArray items = json["items"].toArray();

		foreach(QJsonValue item, items) {
			tracks.append(item);
		}

		if (json["next"].toString() == "") finished = true;
	}

	return tracks;
}