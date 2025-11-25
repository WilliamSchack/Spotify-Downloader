#include "VersionManager.h"

bool VersionManager::UpdateAvailable() {
	QString currentTag = VERSION;

	// Check if already called and return cache if so
	if (_latestVersionCache != nullptr) {
		QString latestTag = _latestVersionCache;

		return VersionHigher(latestTag, currentTag);
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

	// Store latest version for later calls
	_latestVersionCache = latestTag;

	return VersionHigher(latestTag, currentTag);
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

bool VersionManager::VersionHigher(const QString& latestVersion, const QString& currentVersion) {
	// Seperate parts of the version
	QStringList latestVersionSplit = QString(latestVersion).remove("v").split(".");
	QStringList currentVersionSplit = QString(currentVersion).remove("v").split(".");

	unsigned int latestVersionSplitSize = latestVersionSplit.size();
	unsigned int currentVersionSplitSize = currentVersionSplit.size();
	unsigned int largestSize = latestVersionSplitSize > currentVersionSplitSize ? latestVersionSplitSize : currentVersionSplitSize;

	// Check each part of the version
	for (unsigned int i = 0; i < largestSize; i++) {
		// If latest is vX.X.X.Y and current is vX.X.X, there is a hotfix
		if (i >= currentVersionSplitSize)
			return true;

		// If current is vX.X.X.Y and latest is vX.X.X, there is no hotfix
		if (i >= latestVersionSplitSize)
			return false;

		// If any version part is larger than the current, the version is higher
		int latestVersionPart = latestVersionSplit[i].toInt();
		int currentVersionPart = currentVersionSplit[i].toInt();

		qDebug() << "Checking:" << latestVersionPart << currentVersionPart << (latestVersionPart > currentVersionPart) << (currentVersionPart > latestVersionPart);

		if (latestVersionPart > currentVersionPart)
			return true;
		else if (currentVersionPart > latestVersionPart)
			return false;
	}

	return false;
}