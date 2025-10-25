#ifndef VERSIONMANAGER_H
#define VERSIONMANAGER_H

#include "Network/Network.h"

#include <QString>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

class VersionManager {
	public:
		static inline const QString VERSION = "1.10.7";
	public:
		static bool UpdateAvailable();
		static QString LatestVersion();
	private:
		static inline const QString GITHUB_TAGS_URL = "https://api.github.com/repos/WilliamSchack/Spotify-Downloader/tags";

		static inline QString _latestVersionCache = nullptr;
	private:
		static bool VersionHigher(const QString& latestVersion, const QString& currentVersion);
};

#endif