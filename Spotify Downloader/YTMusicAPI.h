#ifndef YTMUSICAPI_H
#define YTMUSICAPI_H

#include <time.h>
#include <regex>

#include <QObject>
#include <QTime>

#include "JSONUtils.h"
#include "Network.h"

// Translation from https://github.com/sigma67/ytmusicapi
// Only contains things relevant to this project
// A lot has been cut out of it so dont expect everything to be here

class YTMusicAPI {
	public:
		YTMusicAPI();

		QJsonArray Search(QString query, QString filter, int limit);
	private:
		QJsonObject ParseSongRuns(QJsonArray runs, int offset = 0);
		QJsonArray ParseSearchResults(QJsonArray results, QString resultType = "", QString catagory = "");
		QString GetItemText(QJsonObject item, int index = 0, int runIndex = 0);
		QJsonObject GetFlexColumnItem(QJsonObject item, int index);
		int TimeToSeconds(QString time);

		QNetworkRequest _request;
		QJsonObject _context;
};

#endif