// Translation from https://github.com/Strvm/musicxmatch-api
// Only contains things relevant to this project

#ifndef MUSIXMATCHAPI_H
#define MUSIXMATCHAPI_H

#define QT_MESSAGELOGCONTEXT
#include "Utilities/Logger.h"

#include "Network/Network.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QRegularExpression>

#include <QMessageAuthenticationCode>

class MusixmatchAPI {
	public:
		class SynchronisedLyric {
			public:
				int StartMs;
				int EndMs;
				QString Lyric;
			public:
				SynchronisedLyric(int startMs, int endMs, QString lyric) {
					StartMs = startMs;
					EndMs = endMs;
					Lyric = lyric;
				}
		};

		enum LyricsType {
			None,
			Unsynced,
			Synced
		};
	public:
		static QJsonObject GetTrack(QString isrc);

		static LyricsType GetLyricType(QString isrc);
		static QString GetLyrics(QString isrc);
		static QList<SynchronisedLyric> GetSyncedLyrics(QString isrc);
	private:
		// Status code descriptions from "https://docs.musixmatch.com/lyrics-api/api-methods"
		static inline const QMap<int, QString> STATUS_CODE_DESCRIPTIONS = {
			{200, "The request was successful."},
			{400, "The request had bad syntax or was inherently impossible to be satisfied."},
			{401, "Authentication failed, probably because of invalid/missing API key. (Most likely rate limited)"},
			{402, "The usage limit has been reached, either you exceeded per day requests limits or your balance is insufficient."},
			{403, "You are not authorized to perform this operation."},
			{404, "The requested resource was not found."},
			{405, "The requested method was not found."},
			{500, "Oops, something went wrong."},
			{503, "Our system is a bit busy at the moment and your request can’t be satisfied."}
		};

		static inline const QString BASE_URL = "https://www.musixmatch.com/ws/1.1/";

		static inline QString _latestAppURLCache;
		static inline QByteArray _secretCache;
	private:
		static void AddHeaders(QNetworkRequest& request);

		static QString GetLatestAppURL();
		static QByteArray GetSecret();
		static QString GenerateSignature(QString url);

		static QJsonObject Request(QString endpoint, QString isrc);
		static QString GetStatusCodeLog(int statusCode);
};

#endif