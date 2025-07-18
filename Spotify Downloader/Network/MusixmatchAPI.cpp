#include "MusixmatchAPI.h"

void MusixmatchAPI::AddHeaders(QNetworkRequest& request) {
	request.setRawHeader("user-agent", "Mozilla/5.0");
	request.setRawHeader("cookie", "mxm_bab=AB");
}

QString MusixmatchAPI::GetLatestAppURL() {
	// If already called, return cache
	if (!_latestAppURLCache.isEmpty())
		return _latestAppURLCache;

	// Get the search page
	QUrl url("https://www.musixmatch.com/search");

	QNetworkRequest request(url);
	AddHeaders(request);

	QByteArray response = Network::Get(request);

	// Find the "_app" script URLs
	QRegularExpression regex(R"#(src="([^"]*/_next/static/chunks/pages/_app-[^"]+\.js)")#");
	QStringList matches = regex.match(response).capturedTexts();

	// If no matches where found, return nothing
	if (matches.count() == 0) {
		qWarning() << "Could not find _app URL";
		return "";
	}

	// Return last match
	_latestAppURLCache = matches.last();
	return _latestAppURLCache;
}

QByteArray MusixmatchAPI::GetSecret() {
	// If already called, return cache
	if (!_secretCache.isEmpty())
		return _secretCache;

	// Get the latest app url
	QUrl url(GetLatestAppURL());

	// If url is not found, return nothing
	if (url.isEmpty())
		return "";

	// Get the app url javascript
	QNetworkRequest request(url);
	AddHeaders(request);

	QByteArray response = Network::Get(request);

	// Get the string inside "from(....split"
	QRegularExpression regex(R"#(from\(\s*"(.*?)"\s*\.split)#");
	QStringList matches = regex.match(response).capturedTexts();

	// If no match was found, return nothing
	if (matches.count() < 2) {
		qWarning() << "Could not find the secret string";
		return "";
	}

	// Get the encoded string
	QString encodedString = matches[1];

	// Reverse the string
	std::reverse(encodedString.begin(), encodedString.end());

	// Decode reversed from base64
	QByteArray decodedString = QByteArray::fromBase64(encodedString.toUtf8());

	// Return the decoded string
	_secretCache = decodedString;
	return _secretCache;
}

QString MusixmatchAPI::GenerateSignature(QString url) {
	// Get the current time
	std::time_t t = std::time(0);
	std::tm* now = std::localtime(&t);
	QString year = QString::number(now->tm_year + 1900);
	QString month = QString::number(now->tm_mon + 1).rightJustified(2, '0');
	QString day = QString::number(now->tm_mday).rightJustified(2, '0');

	// Hash the url and date with the secret as the key with sha256
	QByteArray message = QString(url + year + month + day).toUtf8();
	QByteArray key = GetSecret();

	QByteArray hashOutput = QMessageAuthenticationCode::hash(message, key, QCryptographicHash::Sha256);
	QByteArray hashOutputBase64 = hashOutput.toBase64();
	QByteArray urlEncodedHash = QUrl::toPercentEncoding(hashOutputBase64);

	// Return the signature url params
	return QString("&signature=%1&signature_protocol=sha256").arg(urlEncodedHash);
}

QJsonObject MusixmatchAPI::Request(QString endpoint, QString isrc) {
	// Create the url
	QString urlString = QString("%1%2?app_id=web-desktop-app-v1.0&format=json&track_isrc=%3").arg(BASE_URL).arg(endpoint).arg(isrc);
	urlString = urlString + GenerateSignature(urlString);

	// Get the data from musixmatch
	QUrl url(urlString);

	QNetworkRequest request(url);
	AddHeaders(request);

	QByteArray response = Network::Get(request);
	return QJsonDocument::fromJson(response).object();
}

void MusixmatchAPI::LogStatusCode(int statusCode, QString prefixLog) {
	ErrorSeverity severity = STATUS_CODE_DETAILS[statusCode].Severity;
	QString description = QString("(%1) %2").arg(statusCode).arg(STATUS_CODE_DETAILS[statusCode].Description);

	// Create print lambda for easier calling
	auto printStatusCode = [&prefixLog, &severity, &description]() {
		switch (severity) {
			case ErrorSeverity::Warning:
				qWarning() << prefixLog << description;
				break;
			case ErrorSeverity::Info:
				qInfo() << prefixLog << description;
				break;
		}
	};

	switch (ErrorLoggingType) {
		case LoggingType::All:
			printStatusCode();
			break;
		case LoggingType::Warnings:
			if(severity == ErrorSeverity::Warning)
				printStatusCode();
			break;
		case LoggingType::Info:
			if (severity == ErrorSeverity::Info)
				printStatusCode();
			break;
	}
}

QJsonObject MusixmatchAPI::GetTrack(QString isrc) {
	// Get the track from musixmatch
	QJsonObject response = Request("track.get", isrc);

	// Check for any errors
	QJsonObject message = response["message"].toObject();
	QJsonObject header = message["header"].toObject();
	int statusCode = header["status_code"].toInt();

	LogStatusCode(statusCode, QString("(%1) Musixmatch failed to get track with the error:").arg(isrc));
	if (statusCode != 200)
		return QJsonObject();

	// Return the track
	return message["body"].toObject()["track"].toObject();
}

Lyrics MusixmatchAPI::GetLyrics(QString isrc) {
	Lyrics lyrics;

	lyrics.Type = GetLyricType(isrc);
	lyrics.SourceMessage = QString("Musixmatch: %1").arg(isrc).toStdString();

	switch (lyrics.Type) {
		case Lyrics::LyricsType::Unsynced:
			lyrics.UnsyncedLyrics = GetUnsyncedLyrics(isrc);
			break;
		case Lyrics::LyricsType::Synced:
			lyrics.SyncedLyrics = GetSyncedLyrics(isrc);
			break;
	}

	return lyrics;
}

Lyrics::LyricsType MusixmatchAPI::GetLyricType(QString isrc) {
	// Get the track
	QJsonObject track = GetTrack(isrc);

	// Return the type of lyrics available
	bool hasLyrics = track["has_lyrics"].toInt() == 1;
	bool hasSyncedLyrics = track["has_richsync"].toInt() == 1;

	if (hasSyncedLyrics)
		return Lyrics::LyricsType::Synced;

	if (hasLyrics)
		return Lyrics::LyricsType::Unsynced;

	return Lyrics::LyricsType::None;
}

std::string MusixmatchAPI::GetUnsyncedLyrics(QString isrc) {
	// Get the lyrics from musixmatch
	QJsonObject response = Request("track.lyrics.get", isrc);

	// Check for any errors
	QJsonObject message = response["message"].toObject();
	QJsonObject header = message["header"].toObject();
	int statusCode = header["status_code"].toInt();

	LogStatusCode(statusCode, QString("(%1) Musixmatch failed to get lyrics with the error:").arg(isrc));
	if (statusCode != 200)
		return "";

	// Return the lyrics
	QJsonObject lyrics = message["body"].toObject()["lyrics"].toObject();
	return lyrics["lyrics_body"].toString().toStdString();
}

std::list<Lyrics::SynchronisedLyric> MusixmatchAPI::GetSyncedLyrics(QString isrc) {
	// Get the synced lyrics from musixmatch
	QJsonObject response = Request("track.richsync.get", isrc);

	// Check for any errors
	QJsonObject message = response["message"].toObject();
	QJsonObject header = message["header"].toObject();
	int statusCode = header["status_code"].toInt();

	LogStatusCode(statusCode, QString("(%1) Musixmatch failed to get synced lyrics with the error:").arg(isrc));
	if (statusCode != 200)
		return std::list<Lyrics::SynchronisedLyric>();

	// Get the lyrics from the response
	QJsonObject richSync = message["body"].toObject()["richsync"].toObject();
	QString lyricsString = richSync["richsync_body"].toString();
	QJsonArray lyricsArray = QJsonDocument::fromJson(lyricsString.toUtf8()).array();

	std::list<Lyrics::SynchronisedLyric> lyricsList;
	foreach(QJsonValue lyricDataValue, lyricsArray) {
		QJsonObject lyricData = lyricDataValue.toObject();

		// Get the sentences rather than specific words
		int startMs = lyricData["ts"].toDouble() * 1000;
		int endMs = lyricData["te"].toDouble() * 1000;
		std::string sentence = lyricData["x"].toString().toStdString();

		Lyrics::SynchronisedLyric lyric(startMs, endMs, sentence);
		lyricsList.push_back(lyric);
	}

	return lyricsList;
}