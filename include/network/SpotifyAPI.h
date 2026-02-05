#ifndef SPOTIFYAPI_H
#define SPOTIFYAPI_H

#include "NetworkRequest.h"

#include <QObject>
#include <QByteArray>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QFuture>
#include <QtConcurrent/QtConcurrentRun>

class SpotifyAPI {
	public:
		SpotifyAPI();

		static inline QByteArray ClientID;
		static inline QByteArray ClientSecret;

		bool CheckConnection();
		QJsonObject GetPlaylist(QString id);
		QJsonArray GetPlaylistTracks(QString id);
		QJsonObject GetAlbum(QString id);
		QJsonArray GetAlbumTracks(QJsonObject album);
		QJsonObject GetTrack(QString id);
		QJsonObject GetEpisode(QString id);
	private:
		const QByteArray DEFAULT_CLIENT_ID = "DEFAULT_CLIENT_ID";
		const QByteArray DEFAULT_CLIENT_SECRET = "DEFAULT_CLIENT_SECRET";

		QByteArray _auth;
	private:
		QJsonArray GetTracks(QJsonObject json);
};

#endif