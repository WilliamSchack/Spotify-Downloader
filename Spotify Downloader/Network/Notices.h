#ifndef NOTICESMANAGER_H
#define NOTICESMANAGER_H

#define QT_MESSAGELOGCONTEXT
#include "Utilities/Logger.h"

#include "Network.h"

#include <string>
#include <vector>

struct Notice {
	int id;
	std::string date;
	std::string content;
};

class NoticesManager {
	public:
		static inline int LastReadNoticeID = -1;
	public:
		static std::vector<Notice> GetLatestNotices();
		static void ReadNotice(int noticeId);
	private:
		static inline const std::string NOTICES_SOURCE = "https://data.wilschack.dev/SpotifyDownloader/notices.json";

		static inline std::vector<Notice> _currentNotices;
	private:
		static void UpdateNotices();
};

#endif