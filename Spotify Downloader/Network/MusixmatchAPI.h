// Translation from https://github.com/Strvm/musicxmatch-api
// Only contains things relevant to this project

#ifndef MUSIXMATCHAPI_H
#define MUSIXMATCHAPI_H

#define QT_MESSAGELOGCONTEXT
#include "Utilities/Logger.h"

#include "Network/Network.h"
#include "Lyrics/Lyrics.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QRegularExpression>

#include <QMessageAuthenticationCode>

class MusixmatchAPI {
	public:
		enum class LoggingType {
			All,
			Warnings,
			Info,
			None
		};

		enum class ErrorSeverity {
			Warning,
			Info
		};

		struct StatusCodeDetails {
			ErrorSeverity Severity;
			QString Description;
		};
	public:
		static QJsonObject GetTrack(QString isrc, LoggingType loggingType = LoggingType::All);

		static Lyrics GetLyrics(QString isrc, LoggingType loggingType = LoggingType::All);
		static Lyrics::LyricsType GetLyricType(QString isrc, LoggingType loggingType = LoggingType::All);
		static std::string GetUnsyncedLyrics(QString isrc, LoggingType loggingType = LoggingType::All);
		static std::list<Lyrics::SynchronisedLyric> GetSyncedLyrics(QString isrc, LoggingType loggingType = LoggingType::All);
	private:
		// Status code descriptions from "https://docs.musixmatch.com/lyrics-api/api-methods"
		static inline const QMap<int, StatusCodeDetails> STATUS_CODE_DETAILS = {
			{ 200, { ErrorSeverity::Info, "The request was successful." } },
			{ 400, { ErrorSeverity::Warning, "The request had bad syntax or was inherently impossible to be satisfied." } },
			{ 401, { ErrorSeverity::Warning, "Authentication failed, probably because of invalid/missing API key. (Most likely bot prevention)" } },
			{ 402, { ErrorSeverity::Warning, "The usage limit has been reached, either you exceeded per day requests limits or your balance is insufficient." } },
			{ 403, { ErrorSeverity::Warning, "You are not authorized to perform this operation." } },
			{ 404, { ErrorSeverity::Info, "The requested resource was not found." } },
			{ 405, { ErrorSeverity::Info, "The requested method was not found." } },
			{ 500, { ErrorSeverity::Warning, "Oops, something went wrong." } },
			{ 503, { ErrorSeverity::Warning, "Our system is a bit busy at the moment and your request can’t be satisfied." } }
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
		static void LogStatusCode(int statusCode, QString prefixLog, LoggingType loggingType = LoggingType::All);
};

#endif