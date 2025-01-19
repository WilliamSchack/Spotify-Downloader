#ifndef SPOTIFYAPI_H
#define SPOTIFYAPI_H

#define QT_MESSAGELOGCONTEXT
#include "Utilities/Logger.h"

#include "Network/Network.h"

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

		bool CheckConnection();
		QJsonArray GetPlaylistTracks(QString id);
		QJsonObject GetAlbum(QString id);
		QJsonArray GetAlbumTracks(QJsonObject album);
		QJsonObject GetTrack(QString id);
		QJsonObject GetEpisode(QString id);
	private:
		const QByteArray CLIENT_ID = "CLIENT_ID";
		const QByteArray CLIENT_SECRET = "CLIENT_SECRET";

		QByteArray _auth;
	private:
		QJsonArray GetTracks(QJsonObject json);
};

#endif