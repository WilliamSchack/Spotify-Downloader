#include "MusixmatchAPI.h"

QString MusixmatchAPI::GetLatestAppURL() {
	// If already called, return cache
	if (!_latestAppURLCache.isEmpty())
		return _latestAppURLCache;

	// Get the search page
	QUrl url("https://www.musixmatch.com/search");

	QNetworkRequest request(url);
	request.setRawHeader("user-agent", "Mozilla/5.0");
	request.setRawHeader("cookie", "mxm_bab=AB");

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
	request.setRawHeader("user-agent", "Mozilla/5.0");
	request.setRawHeader("cookie", "mxm_bab=AB");

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

	// Return the signature url part
	return QString("&signature=%1&signature_protocol=sha256").arg(urlEncodedHash);
}