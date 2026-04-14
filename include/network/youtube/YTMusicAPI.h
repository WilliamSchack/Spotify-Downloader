// Translation from https://github.com/sigma67/ytmusicapi
// Only contains things relevant to this project
// Implementation here is a bit scuffed but it works

#ifndef YTMUSICAPI_H
#define YTMUSICAPI_H

#include "EYoutubeCategory.h"
#include "YoutubeSearchResult.h"
#include "NetworkRequest.h"
#include "PlaylistTracks.h"
#include "AlbumTracks.h"
#include "Lyrics.h"
#include "StringUtils.h"
#include "ArrayUtils.h"
#include "JsonUtils.h"

#include <chrono>
#include <regex>

#include <iostream>
#include <QObject>
#include <QTime>
#include <QRegularExpression>
#include <QJsonArray>

class YTMusicAPI {
	public:
		bool CheckConnection();
		
		std::vector<YoutubeSearchResult> Search(const std::string& query, const EYoutubeCategory& filter, int limit);

		TrackData GetTrack(const std::string& videoId);

		std::string GetAlbumBrowseId(const std::string& playlistId);
		AlbumTracks GetAlbum(const std::string& browseId);

		PlaylistTracks GetPlaylist(const std::string& playlistId);

		Lyrics GetLyrics(const std::string& videoId, const bool& timestamps = true);

		//bool HasPremium(QString cookies);

		//bool IsAgeRestricted(QString videoId);
	private:
		NetworkRequest GetRequestURL(const std::string& url);
		NetworkRequest GetRequestAPI(const std::string& endpoint);
		nlohmann::json GetContext();

		TrackData ParseTrackJson(const nlohmann::json& json);
		ArtistData ParseArtistJson(const nlohmann::json& json);
		AlbumTracks ParseAlbumJson(const nlohmann::json& json);

		nlohmann::json GetWatchPlaylist(std::string id, bool isPlaylist = false);
		nlohmann::json ParseWatchPlaylist(const nlohmann::json& json);
		nlohmann::json ParseWatchTrack(const nlohmann::json& json);

		nlohmann::json ParseSongRuns(const nlohmann::json& runs, const int& offset = 0);

		nlohmann::json ParseAlbumHeader(const nlohmann::json& response);
		nlohmann::json ParsePlaylistItems(const nlohmann::json& results, const bool& isAlbum = false);

		nlohmann::json ParseSongArtists(const nlohmann::json& data, const int& index);
		nlohmann::json ParseSongAlbum(const nlohmann::json& data, const int& index);

		nlohmann::json ParseSearchResults(const nlohmann::json& results, std::string resultType = "", const std::string& category = "");

		std::string GetItemText(const nlohmann::json& item, const int& index = 0, const int& runIndex = 0);
		nlohmann::json GetFlexColumnItem(const nlohmann::json& item, const int& index);
		nlohmann::json GetFixedColumnItem(const nlohmann::json& item, const int& index);

		std::string GetLyricsBrowseId(const std::string& videoId);
		std::string GetTabBrowseId(const nlohmann::json& watchNextRenderer, const int& tabId);

		std::string GetLargestImageUrl(const nlohmann::json& json);

		long GetSignatureTimestamp();
		int TimeToSeconds(const std::string& time);
	private:
		static inline const std::string YOUTUBE_URL = "https://music.youtube.com";
        static inline const std::string API_BASE_URL = YOUTUBE_URL + "/youtubei/v1";
		static inline const std::string VIDEO_BASE_URL = YOUTUBE_URL + "/watch?v=";
		static inline const std::string CHANNEL_BASE_URL = YOUTUBE_URL + "/channel/";
		static inline const std::string BROWSE_BASE_URL = YOUTUBE_URL + "/browse/";
		static inline const std::string PLAYLIST_BASE_URL = YOUTUBE_URL + "/playlist?list=";

		static inline const std::string VALID_PREMIUM_IMAGE_ALT_TEXT[] {
			"YouTube Premium",
			"YouTube Music Premium"
		};
};

#endif