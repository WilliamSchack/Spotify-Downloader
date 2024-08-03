#ifndef SPOTIFYAPI_H
#define SPOTIFYAPI_H

#define QT_MESSAGELOGCONTEXT
#include "Logger.h"

#include <QObject>
#include <QByteArray>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QFuture>
#include <QtConcurrent/QtConcurrentRun>

#include "Network.h"

class SpotifyAPI {
	public:
		SpotifyAPI();

		bool CheckConnection();
		QJsonArray GetPlaylistTracks(QString id);
		QJsonObject GetAlbum(QString id);
		QJsonArray GetAlbumTracks(QJsonObject album);
		QJsonObject GetTrack(QString id);
	private:
		QByteArray _auth;
		QJsonArray GetTracks(QJsonObject json);

		const QByteArray CLIENT_ID = "CLIENT_ID";
		const QByteArray CLIENT_SECRET = "CLIENT_SECRET";
};

#endif