// Translation from https://github.com/sigma67/ytmusicapi
// Only contains things relevant to this project

#ifndef YTMUSICAPI_H
#define YTMUSICAPI_H

#include "NetworkRequest.h"
#include "StringUtils.h"
#include "ArrayUtils.h"
#include "JsonUtils.h"

#include <nlohmann/json.hpp>

#include <time.h>
#include <regex>
#include <format>

#include <iostream>
#include <QObject>
#include <QTime>
#include <QRegularExpression>
#include <QJsonArray>

class YTMusicAPI {
	public:
		bool CheckConnection();
		
		nlohmann::json Search(const std::string&, const std::string&, int limit);

		QJsonObject GetAlbum(QString browseId);
		QJsonArray GetAlbumTracks(QString browseId);

		//Lyrics GetLyrics(QString videoId, bool timestamps = true);

		bool HasPremium(QString cookies);

		bool IsAgeRestricted(QString videoId);
	private:
        static inline const std::string API_BASE_URL = "https://music.youtube.com/youtubei/v1";

		static inline const std::string VALID_PREMIUM_IMAGE_ALT_TEXT[] {
			"YouTube Premium",
			"YouTube Music Premium"
		};
	private:
		NetworkRequest GetRequest(const std::string& endpoint);
		nlohmann::json GetContext();

		nlohmann::json ParseSongRuns(const nlohmann::json& runs, int offset = 0);

		QJsonObject ParseAlbumHeader(QJsonObject response);
		QJsonArray ParsePlaylistItems(QJsonArray results, bool isAlbum = false);

		QJsonArray ParseSongArtists(QJsonObject data, int index);
		QJsonObject ParseSongAlbum(QJsonObject data, int index);

		nlohmann::json ParseSearchResults(const nlohmann::json& results, std::string resultType = "", const std::string& category = "");

		std::string GetItemText(const nlohmann::json& item, int index = 0, int runIndex = 0);
		nlohmann::json GetFlexColumnItem(const nlohmann::json& item, int index);
		QJsonObject GetFixedColumnItem(QJsonObject item, int index);

		QString GetLyricsBrowseId(QString videoId);
		QString GetTabBrowseId(QJsonObject watchNextRenderer, int tabId);

		int TimeToSeconds(const std::string& time);
};

#endif