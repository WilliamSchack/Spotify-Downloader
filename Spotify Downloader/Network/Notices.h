#ifndef NOTICESMANAGER_H
#define NOTICESMANAGER_H

#define QT_MESSAGELOGCONTEXT
#include "Utilities/Logger.h"

#include "Network.h"
#include "Config.h"

#include <string>
#include <vector>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

struct Notice {
	int id;
	std::string date;
	std::string title;
	std::string content;
	bool read;
};

class NoticesManager {
	public:
	public:
		static std::vector<Notice> GetLatestNotices();
		static void ReadNotice(int noticeId);
	private:
		static inline const QUrl NOTICES_SOURCE = QUrl("https://data.wilschack.dev/SpotifyDownloader/notices.json");

		static inline std::vector<Notice> _notices;
		static inline int _lastReadNoticeID = -1;
	private:
		static void UpdateNotices();
		static int GetLastReadNotice();
};

#endif