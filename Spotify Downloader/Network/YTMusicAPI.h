// Translation from https://github.com/sigma67/ytmusicapi
// Only contains things relevant to this project
// A lot has been cut out of it so dont expect everything to be here

#ifndef YTMUSICAPI_H
#define YTMUSICAPI_H

#define QT_MESSAGELOGCONTEXT
#include "Utilities/Logger.h"

#include "Utilities/JSONUtils.h"
#include "Network/Network.h"

#include <time.h>
#include <regex>

#include <QObject>
#include <QTime>

class YTMusicAPI {
	public:
		bool CheckConnection();
		QJsonArray Search(QString query, QString filter, int limit);

		QJsonObject GetAlbum(QString browseId);
		QJsonArray GetAlbumTracks(QString browseId);
	private:
		QNetworkRequest GetRequest(QString endpoint);
		QJsonObject GetContext(); // Just so we get the current time instead of an initialized one

		QJsonObject ParseSongRuns(QJsonArray runs, int offset = 0);

		QJsonObject ParseAlbumHeader(QJsonObject response);
		QJsonArray ParsePlaylistItems(QJsonArray results, bool isAlbum = false);

		QJsonArray ParseSongArtists(QJsonObject data, int index);
		QJsonObject ParseSongAlbum(QJsonObject data, int index);

		QJsonArray ParseSearchResults(QJsonArray results, QString resultType = "", QString catagory = "");

		QString GetItemText(QJsonObject item, int index = 0, int runIndex = 0);
		QJsonObject GetFlexColumnItem(QJsonObject item, int index);
		QJsonObject GetFixedColumnItem(QJsonObject item, int index);

		int TimeToSeconds(QString time);
};

#endif