#include "VersionManager.h"

bool VersionManager::UpdateAvailable() {
	// Check if already called and return cache if so
	if (_latestVersionCache != nullptr) {
		QString latestTag = _latestVersionCache;
		QString currentTag = VERSION;

		// Turn into int
		int latestVersion = latestTag.remove("v").remove(".").toInt();
		int currentVersion = currentTag.remove(".").toInt();

		return latestVersion > currentVersion;
	}

	QUrl url(GITHUB_TAGS_URL);

	// Check connectivity to server
	if (!Network::Ping(url)) {
		qWarning() << "Could not access github api for latest release tag";
		return false;
	}

	// Get latest tag from github
	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

	QByteArray response = Network::Get(request);
	
	if (response == nullptr) {
		qWarning() << "Github API returned error";
		return false;
	}

	QJsonArray json = QJsonDocument::fromJson(response).array();

	QString latestTag = json[0].toObject()["name"].toString();
	QString currentTag = VERSION;

	// Store latest version for later calls
	_latestVersionCache = latestTag;

	// Cast to int
	int latestVersion = latestTag.remove("v").remove(".").toInt();
	int currentVersion = currentTag.remove(".").toInt();

	return latestVersion > currentVersion;
}

QString VersionManager::LatestVersion() {
	if (_latestVersionCache != nullptr)
		return _latestVersionCache;

	// Get latest tag
	UpdateAvailable();

	// Check if server responded
	if(_latestVersionCache != nullptr)
		return _latestVersionCache;

	// Return null if any error
	return "";
}