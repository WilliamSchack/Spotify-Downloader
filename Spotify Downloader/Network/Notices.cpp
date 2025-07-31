#include "Notices.h"

std::vector<Notice> NoticesManager::GetLatestNotices() {
	if (_notices.empty())
		UpdateNotices();

	return _notices;
}

void NoticesManager::UpdateNotices() {
	QNetworkRequest request = QNetworkRequest(NOTICES_SOURCE);
	request.setRawHeader("user-agent", "Mozilla/5.0");

	QByteArray response = Network::Get(request);
	QString responseString = QString(response);
	QJsonArray json = QJsonDocument::fromJson(responseString.toUtf8()).array();

	// Read the json into notices
	_notices.clear();

	int lastReadNoticeId = GetLastReadNotice();
	for (QJsonValue noticeValue : json) {
		QJsonObject noticeJson = noticeValue.toObject();
		
		Notice notice;
		notice.id = noticeJson["id"].toInt();
		notice.date = noticeJson["release_date"].toString().toStdString();
		notice.title = noticeJson["title"].toString().toStdString();
		notice.content = noticeJson["content"].toString().toStdString();
		notice.read = notice.id <= lastReadNoticeId;

		_notices.push_back(notice);
	}
}

void NoticesManager::ReadNotice(int noticeId) {
	_lastReadNoticeID = noticeId;

	// Save to settings
	QSettings settings(Config::ORGANIZATION_NAME, Config::APPLICATION_NAME);
	settings.setValue("lastReadNoticeId", noticeId);
}

int NoticesManager::GetLastReadNotice() {
	if (_lastReadNoticeID != -1)
		return _lastReadNoticeID;

	QSettings settings(Config::ORGANIZATION_NAME, Config::APPLICATION_NAME);
	if (settings.contains("lastReadNoticeId"))
		return settings.value("lastReadNoticeId").toInt();

	return -1;
}