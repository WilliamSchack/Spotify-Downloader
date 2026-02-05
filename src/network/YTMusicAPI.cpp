#include "YTMusicAPI.h"

bool YTMusicAPI::CheckConnection() {
    NetworkRequest request;
    request.Url = API_BASE_URL;
    NetworkResponse response = request.Get();
	
	// If anything is returned, connection is ok
	return response.CurlCode == CURLcode::CURLE_OK;
}

NetworkRequest YTMusicAPI::GetRequest(const std::string& endpoint) {
    NetworkRequest request;
    request.Url = API_BASE_URL + "/" + endpoint + "/?alt=json";
	request.SetHeader("User-Agent", "Mozilla/5.0");
	request.SetHeader("Accept", "*/*");
	request.SetHeader("Accept-encoding", "gzip, deflate");
	request.SetHeader("Content-Type", "application/json");
	request.SetHeader("Content-Encoding", "gzip");
	request.SetHeader("Origin", "https://music.youtube.com/youtubei/v1/");

	return request;
}

nlohmann::json YTMusicAPI::GetContext() {
	std::time_t rawTime;
	struct tm* timeInfo;
	char buffer[80];
	std::time(&rawTime);
	timeInfo = std::localtime(&rawTime);
	std::strftime(buffer, sizeof(buffer), "%Y%m%d", timeInfo);
	std::string clientVersion = "1." + std::string(buffer) + ".01.00";

    return nlohmann::json {
        {"client", {
            {"clientName", "WEB_REMIX"},
            {"clientVersion", clientVersion}
        }},
        {"user", nlohmann::json::object()}
    };
}

nlohmann::json YTMusicAPI::Search(const std::string& query, const std::string& filter, int limit) {
	// Get web page
	std::string searchParams = "EgWKAQ"; // Param 1
	if (filter == "songs") searchParams += "II"; //  Songs Filter
	else if (filter == "videos") searchParams += "IQ"; // Videos Filter
	else if (filter == "albums") searchParams += "IY"; // Albums Filter
	searchParams += "AUICCAFqDBAOEAoQAxAEEAkQBQ%3D%3D"; // Ignore Spelling

	nlohmann::json postData {
		{"query", query},
		{"params", searchParams},
		{"context", GetContext()}
	};

	NetworkRequest request = GetRequest("search");
	NetworkResponse response = request.Post(postData);

	nlohmann::json responseJson = nlohmann::json::parse(response.Body);
	if (!responseJson.contains("contents"))
		return nlohmann::json::object();

	nlohmann::json contents = responseJson["contents"]["tabbedSearchResultsRenderer"]["tabs"][0]["tabRenderer"]["content"]["sectionListRenderer"]["contents"];

	nlohmann::json searchResults;
	for (nlohmann::json result : contents) {
		std::string type;
		std::string category;

		if (result.contains("musicCardShelfRenderer")) {
			nlohmann::json data = result["musicCardShelfRenderer"];

			std::string resultTypes[] { "artist", "playlist", "song", "video", "station", "profile" };
			std::string resultType = data["subtitle"]["runs"][0]["text"];
			StringUtils::ToLower(resultType);

			if (!ArrayUtils::Contains(resultTypes, resultType))
				resultType = "album";
			
			nlohmann::json topResult;
			topResult["resultType"] = resultType;

			nlohmann::json categoryJson = JsonUtils::SafelyNavigate(data, { "header", "musicCardShelfHeaderBasicRenderer", "title" });
			if (!categoryJson.empty())
				category = categoryJson["runs"][0]["text"];

			topResult["category"] = category;

			if (resultType == "song" || resultType == "video") {
				nlohmann::json onTap = data["onTap"];
				if (!onTap.empty()) {
					topResult["videoId"] = onTap["watchEndpoint"]["videoId"];
					topResult["videoType"] = onTap["watchEndpoint"]["watchEndpointMusicSupportedConfigs"]["watchEndpointMusicConfig"]["musicVideoType"];
				}
			}

			if (resultType == "song" || resultType == "video" || resultType == "album") {
				topResult["title"] = data["title"]["runs"][0]["text"];
				nlohmann::json runs = data["subtitle"]["runs"];
				nlohmann::json songInfo = ParseSongRuns(runs, 2);
				topResult.merge_patch(songInfo);

			}

			searchResults.push_back(topResult);

			if (!result["musicCardShelfRenderer"].contains("contents") || contents != result["musicCardShelfRenderer"]["contents"])
				continue;

			contents = result["musicCardShelfRenderer"]["contents"];
			category = "";
			if (contents[0].contains("messageRenderer")) {
				contents.erase(0);
				category = contents[0]["messageRenderer"]["text"]["runs"][0]["text"];
			}

		} else if (result.contains("musicShelfRenderer")) {
			contents = result["musicShelfRenderer"]["contents"];

			nlohmann::json categoryJson = JsonUtils::SafelyNavigate(result, { "musicShelfRenderer", "title" });
			if (!categoryJson.empty())
				category = categoryJson["runs"][0]["text"];

			type = StringUtils::RemoveLast(filter);
			StringUtils::ToLower(type);
		} else {
			continue;
		}

		// Limit search results
		if (contents.size() > limit) {
			while (contents.size() > limit) {
				contents.erase(contents.size() - 1);
			}
		}

		nlohmann::json currentSearchResults = ParseSearchResults(contents, type, category);
		JsonUtils::ExtendArray(searchResults, currentSearchResults);

		if (filter != "") {
			nlohmann::json continuationResults = result["musicShelfRenderer"];
			int currentLimit = limit - searchResults.size();

			nlohmann::json items = nlohmann::json::array();
			while (continuationResults.contains("continuations") && items.size() < currentLimit) {
				std::string ctoken = continuationResults["continuations"][0]["nextContinuationData"]["continuation"];
				std::string additionalParams = "&ctoken=" + ctoken + "&continuation=" + ctoken;

				NetworkRequest continuationRequest = GetRequest("search");
				continuationRequest.Url += additionalParams;
				NetworkResponse continuationResponse = continuationRequest.Post(postData);
				nlohmann::json continuationJson = nlohmann::json::parse(continuationResponse.Body);

				if (continuationJson.contains("continuationContents"))
					continuationResults = continuationJson["continuationContents"]["musicShelfContinuation"];
				else
					break;

				nlohmann::json continuationContents = nlohmann::json::array();
				if (continuationResults.contains("contents"))
					continuationContents = ParseSearchResults(continuationResults["contents"], type, category);
				else if (continuationResults.contains("items"))
					continuationContents = ParseSearchResults(continuationResults["items"], type, category);

				if (continuationContents.size() == 0) break;

				JsonUtils::ExtendArray(items, continuationContents);
			}

			JsonUtils::ExtendArray(searchResults, items);
		}
	}

	return searchResults;
}

/*
QJsonObject YTMusicAPI::GetAlbum(QString browseId) {
	if (!browseId.startsWith("MPRE")) return QJsonObject();

	QJsonObject body {
		{"browseId", browseId},
		{"context", GetContext()}
	};

	QByteArray postData = QJsonDocument(body).toJson();
	QByteArray response = Network::Post(GetRequest("browse"), postData);
	QJsonObject json = QJsonDocument::fromJson(response).object();

	if (!json.contains("contents")) return QJsonObject();

	QJsonObject album = ParseAlbumHeader(json);

	QJsonObject results = JSONUtils::Navigate(json, {"contents", "twoColumnBrowseResultsRenderer", "secondaryContents", "sectionListRenderer", "contents", 0, "musicShelfRenderer"}).toObject();
	QJsonArray tracks = ParsePlaylistItems(results["contents"].toArray(), true);

	for (int i = 0; i < tracks.count(); i++) {
		QJsonObject track = tracks[i].toObject();
		track["album"] = QJsonObject{
			{"id", browseId},
			{"name", album["title"].toString()}
		};
		track["artists"] = track["artists"].toArray().isEmpty() ? album["artists"] : track["artists"];

		track["resultType"] = track["videoType"].toString().contains("MUSIC_VIDEO") ? "video" : ""; // It is a video 99% of the time, just in case it isnt

		tracks[i] = track;
	}

	album["tracks"] = tracks;

	return album;
}

QJsonArray YTMusicAPI::GetAlbumTracks(QString browseId) {
	QJsonObject album = GetAlbum(browseId);
	if (album.isEmpty()) return QJsonArray();

	return album["tracks"].toArray();
}

QJsonObject YTMusicAPI::ParseAlbumHeader(QJsonObject response) {
	QJsonObject header = JSONUtils::Navigate(response, { "contents", "twoColumnBrowseResultsRenderer", "tabs", 0, "tabRenderer", "content", "sectionListRenderer", "contents", 0, "musicResponsiveHeaderRenderer" }).toObject();

	if (header.isEmpty()) return QJsonObject();

	QJsonObject album {
		{"title", header["title"].toObject()["runs"].toArray()[0].toObject()["text"].toString()},
		{"type", header["subtitle"].toObject()["runs"].toArray()[0].toObject()["text"].toString()}
	};

	if (header.contains("description"))
		album["description"] = JSONUtils::Navigate(header, { "description", "musicDescriptionShelfRenderer", "description", "runs", 0, "text" }).toString();
	
	QJsonObject albumInfo = ParseSongRuns(header["subtitle"].toObject()["runs"].toArray());
	if (!header["straplineTextOne"].toObject().isEmpty()) { // If this does not pass, artists will be found for tracks individually, only happens for very few albums
		albumInfo["artists"] = QJsonArray{
			QJsonObject {
				{"name", JSONUtils::Navigate(header, {"straplineTextOne", "runs", 0, "text"}).toString()},
				{"id", JSONUtils::Navigate(header, {"straplineTextOne", "runs", 0, "navigationEndpoint", "browseEndpoint", "browseId"}).toString()}
			}
		};
	}
	album = JSONUtils::Merge(album, albumInfo);

	if (JSONUtils::Navigate(header, {"secondSubtitle", "runs"}).toArray().count() > 1) {
		album["trackCount"] = JSONUtils::Navigate(header, {"secondSubtitle", "runs", 0, "text"}).toString().split(" ")[0].toInt(); // Output is -- Songs, remove " songs" and convert to int
		album["duration"] = JSONUtils::Navigate(header, { "secondSubtitle", "runs", 2, "text" }).toString();
	} else {
		album["duration"] = JSONUtils::Navigate(header, { "secondSubtitle", "runs", 0, "text" }).toString();
	}

	QJsonArray buttons = header["buttons"].toArray();
	album["audioPlaylistId"] = JSONUtils::Navigate(buttons, { 1, "musicPlayButtonRenderer", "playNavigationEndpoint", "watchEndpoint", "playlistId" }).toString();

	bool isExplicit = false;
	if (header.contains("description"))
		isExplicit = JSONUtils::Navigate(header, {"description", "musicDescriptionShelfRenderer"}).toObject().contains("straplineBadge");

	album["isExplicit"] = isExplicit;

	return album;
}

QJsonArray YTMusicAPI::ParsePlaylistItems(QJsonArray results, bool isAlbum) {
	QJsonArray songs = QJsonArray();

	foreach(QJsonValue val, results) {
		QJsonObject result = val.toObject();

		if (!result.contains("musicResponsiveListItemRenderer"))
			continue;

		QJsonObject data = result["musicResponsiveListItemRenderer"].toObject();
		QJsonObject song = QJsonObject();

		if (data.contains("menu")) {
			QJsonArray menuItems = data["menu"].toObject()["menuRenderer"].toObject()["items"].toArray();
			foreach(QJsonValue val, menuItems) {
				QJsonObject	item = val.toObject();

				if (item.contains("menuServiceItemRenderer")) {
					QJsonObject menuService = JSONUtils::Navigate(item, { "menuServiceItemRenderer", "menuServiceItemRenderer" }).toObject();
					if (menuService.contains("playlistEditEndpoint")) {
						song["setVideoId"] = JSONUtils::Navigate(menuService, { "playlistEditEndpoint", "actions", 0, "setVideoId" }).toString();
						song["videoId"] = JSONUtils::Navigate(menuService, { "playlistEditEndpoint", "actions", 0, "removedVideoId" }).toString();
					}
				}
			}
		}

		if (!JSONUtils::Navigate(data, { "overlay", "musicItemThumbnailOverlayRenderer", "content", "musicPlayButtonRenderer" }).isNull())
			song["videoId"] = JSONUtils::Navigate(data, { "overlay", "musicItemThumbnailOverlayRenderer", "content", "musicPlayButtonRenderer", "playNavigationEndpoint", "watchEndpoint", "videoId" }).toString();

		bool isAvailable = true;
		if (data.contains("musicItemRendererDisplayPolicy")) {
			isAvailable = data["musicItemRendererDisplayPolicy"].toString() != "MUSIC_ITEM_RENDERER_DISPLAY_POLICY_GREY_OUT";
		}

		bool usePresetColumns = isAvailable == false || isAlbum == true;

		int titleIndex = usePresetColumns ? 0 : -1;
		int artistIndex = usePresetColumns ? 1 : -1;
		int albumIndex = usePresetColumns ? 2 : -1;
		
		QList<int> userChannelIndexes = QList<int>();
		int unrecognisedIndex = -1;

		foreach(QJsonValue val, data["flexColumns"].toArray()) {
			int index = val.toInt();

			QJsonObject flexColumnItem = GetFlexColumnItem(data, index);

			if (JSONUtils::Navigate(flexColumnItem, { "text", "runs", 0, "navigationEndpoint" }).isNull()) {
				if (!JSONUtils::Navigate(flexColumnItem, { "text", "runs", 0, "text" }).isNull()) {
					unrecognisedIndex = unrecognisedIndex == -1 ? index : unrecognisedIndex;
				}

				continue;
			}

			QJsonObject navigationEndpoint = JSONUtils::Navigate(flexColumnItem, { "text", "runs", 0, "navigationEndpoint" }).toObject();

			if (navigationEndpoint.contains("watchEndpoint")) {
				titleIndex = index;
			} else if (navigationEndpoint.contains("browseEndpoint")) {
				QString pageType = JSONUtils::Navigate(navigationEndpoint, { "browseEndpoint", "browseEndpointContectSupportedConfigs", "browseEndpointContextMusicConfig", "pageType" }).toString();

				if (pageType == "MUSIC_PAGE_TYPE_ARTIST" || pageType == "MUSIC_PAGE_TYPE_UNKNOWN")
					artistIndex = index;
				else if (pageType == "MUSIC_PAGE_TYPE_ALBUM")
					albumIndex = index;
				else if (pageType == "MUSIC_PAGE_TYPE_USER_CHANNEL")
					userChannelIndexes.append(index);
				else if (pageType == "MUSIC_PAGE_TYPE_NON_MUSIC_AUDIO_TRACK_PAGE")
					titleIndex = index;
			}
		}

		if (artistIndex == -1 && unrecognisedIndex != -1)
			artistIndex = unrecognisedIndex;

		if (artistIndex == -1 && !userChannelIndexes.isEmpty())
			artistIndex = userChannelIndexes.last();

		QString title = titleIndex != -1 ? GetItemText(data, titleIndex) : "";
		if (title == "Song deleted")
			return QJsonArray();
		song["title"] = title;

		song["artists"] = artistIndex != -1 ? ParseSongArtists(data, artistIndex) : QJsonArray();

		song["album"] = albumIndex != -1 ? ParseSongAlbum(data, albumIndex) : QJsonObject();

		song["views"] = isAlbum ? GetItemText(data, 2).split(" ")[0] : "";

		song["isExplicit"] = data.contains("badges");

		if (data.contains("fixedColumns")) {
			QString duration = "";
			QJsonObject fixedColumnItemText = GetFixedColumnItem(data, 0)["text"].toObject();
			if (fixedColumnItemText.contains("simpleText"))
				duration = fixedColumnItemText["simpleText"].toString();
			else
				duration = JSONUtils::Navigate(fixedColumnItemText, { "runs", 0, "text" }).toString();

			song["duration"] = duration;
			song["durationSeconds"] = TimeToSeconds(duration);
		}

		song["videoType"] = "";
		if (data.contains("menu"))
			song["videoType"] = JSONUtils::Navigate(data, { "menu", "menuRenderer", "items", 0, "menuNavigationItemRenderer", "navigationEndpoint", "watchEndpoint", "watchEndpointMusicSupportedConfigs", "watchEndpointMusicConfig", "musicVideoType" }).toString();

		songs.append(song);
	}

	return songs;
}
*/

nlohmann::json YTMusicAPI::ParseSongRuns(const nlohmann::json& runs, int offset) {
	nlohmann::json parsed = nlohmann::json::object();
	nlohmann::json artists = nlohmann::json::array();
	for (int i = offset; i < runs.size(); i++) {
		if (i % 2) continue;

		nlohmann::json run = runs[i];
		std::string text = run["text"];

		if (run.contains("navigationEndpoint")) {
			nlohmann::json item {
				{"name", text},
				{"id", run["navigationEndpoint"]["browseEndpoint"]["browseId"]}
			};

			if (item.contains("id") && StringUtils::StartsWith(item["id"], "MPRE") || StringUtils::Contains(item["id"], "release_detail")) {
				parsed["album"] = item;
			}
			else {
				artists.push_back(item);
			}
		}
		else {
			if (std::regex_match(text.c_str(), std::regex(R"(^\d([^ ])* [^ ]*$)")) && i > 2) {
				parsed["views"] = StringUtils::Split(text, " ")[0];
			}
			else if (std::regex_match(text.c_str(), std::regex(R"(^(\d+:)*\d+:\d+$)"))) {
				parsed["duration"] = text;
				parsed["durationSeconds"] = TimeToSeconds(text);
			}
			else if (std::regex_match(text.c_str(), std::regex(R"(^\d{4}$)"))) {
				parsed["year"] = text;
			}
			else {
				// Sometimes views are passed here, check for that and if so set the views
				std::smatch matches;
				if (std::regex_search(text, matches, std::regex(R"((.+)\sviews)"))) {
					parsed["views"] = matches[1];
					continue;
				}
				
				artists.push_back(nlohmann::json {
					{"name", text},
					{"id", ""}
				});
			}
		}
	}
	parsed["artists"] = artists;
	return parsed;
}

int YTMusicAPI::TimeToSeconds(const std::string& time) {
	std::vector<std::string> seperated = StringUtils::Split(time, ":");
	int seconds = 0;
	switch (seperated.size()) {
		case 1:
			seconds = std::stoi(seperated[0]);
			break;
		case 2:
			seconds = std::stoi(seperated[0]) * 60 + std::stoi(seperated[1]);
			break;
		case 3:
			seconds = std::stoi(seperated[0]) * 3600 + std::stoi(seperated[1]) * 60 + std::stoi(seperated[2]);
			break;
		case 4:
			seconds = std::stoi(seperated[0]) * 86400 + std::stoi(seperated[1]) * 3600 + std::stoi(seperated[2]) * 60 + std::stoi(seperated[3]);
			break;
	}

	return seconds;
}

std::string YTMusicAPI::GetItemText(const nlohmann::json& item, int index, int runIndex) {
	nlohmann::json column = GetFlexColumnItem(item, index);
	if (column.empty()) return "";

	return column["text"]["runs"][runIndex]["text"];
}

nlohmann::json YTMusicAPI::GetFlexColumnItem(const nlohmann::json& item, int index) {
	nlohmann::json flexColumns = item["flexColumns"];
	if (flexColumns.size() <= index ||
		!flexColumns[index]["musicResponsiveListItemFlexColumnRenderer"].contains("text") ||
		!flexColumns[index]["musicResponsiveListItemFlexColumnRenderer"]["text"].contains("runs")) {
		return nlohmann::json::object();
	}

	return flexColumns[index]["musicResponsiveListItemFlexColumnRenderer"];
}

/*
QJsonObject YTMusicAPI::GetFixedColumnItem(QJsonObject item, int index) {
	if (!JSONUtils::Navigate(item, { "fixedColumns", index, "musicResponsiveListItemFixedColumnRenderer" }).toObject().contains("text") ||
		!JSONUtils::Navigate(item, { "fixedColumns", index, "musicResponsiveListItemFixedColumnRenderer", "text" }).toObject().contains("runs")) {
		return QJsonObject();
	}

	return JSONUtils::Navigate(item, { "fixedColumns", index, "musicResponsiveListItemFixedColumnRenderer"}).toObject();
}

QJsonArray YTMusicAPI::ParseSongArtists(QJsonObject data, int index) {
	QJsonObject flexItem = GetFlexColumnItem(data, index);

	if (flexItem.isEmpty())
		return QJsonArray();
	
	QJsonArray runs = flexItem["text"].toObject()["runs"].toArray();

	QJsonArray artists = QJsonArray();
	for (int i = 0; i < int(runs.count() / 2 + 1); i++) {
		QString name = JSONUtils::Navigate(runs, { i * 2, "text" }).toString();
		QString id = JSONUtils::Navigate(runs, { i * 2, "navigationEndpoint", "browseEndpoint", "browseId" }).toString();

		QJsonObject artist{
			{ "name", name },
			{ "id", id }
		};

		artists.append(artist);
	}

	return artists;
}

QJsonObject YTMusicAPI::ParseSongAlbum(QJsonObject data, int index) {
	QJsonObject flexItem = GetFlexColumnItem(data, index);

	if (flexItem.isEmpty())
		return QJsonObject();
	
	QString browseId = JSONUtils::Navigate(flexItem, { "text", "runs", 0, "navigationEndpoint", "browseEndpoint", "browseId" }).toString();

	return QJsonObject {
		{ "name", GetItemText(data, index) },
		{ "id", browseId }
	};
}
*/

nlohmann::json YTMusicAPI::ParseSearchResults(const nlohmann::json& results, std::string resultType, const std::string& category) {
	nlohmann::json finalResults = nlohmann::json::array();

	int defaultOffset = resultType.empty() ? 2 : 0;

	for (nlohmann::json val : results) {
		nlohmann::json data = val["musicResponsiveListItemRenderer"];

		nlohmann::json searchResult {
			{"category", category}
		};

		nlohmann::json videoTypeJson = JsonUtils::SafelyNavigate(data, { "overlay", "musicItemThumbnailOverlayRenderer", "content", "musicPlayButtonRenderer", "playNavigationEndpoint", "watchEndpoint", "watchEndpointMusicSupportedConfigs", "watchEndpointMusicConfig", "musicVideoType" });
		std::string videoType = "";
		if (!videoTypeJson.empty())
			videoType = videoTypeJson;
		
		if (resultType == "" && videoType != "") {
			if (videoType == "MUSIC_VIDEO_TYPE_ATV") resultType = "song";
			else resultType = "video";
		}

		if (resultType == "") {
			std::string resultTypes[] { "artist", "playlist", "song", "video", "station", "profile" };
			std::string resultTypeLocal = GetItemText(data, 1);
			StringUtils::ToLower(resultTypeLocal);
			if (!ArrayUtils::Contains(resultTypes, resultTypeLocal)) {
				resultType = "album";
			} else {
				resultType = resultTypeLocal;
			}
		}
		searchResult["resultType"] = resultType;

		if (resultType != "artist") {
			searchResult["title"] = GetItemText(data, 0);
		}

		if (resultType == "artist")
			searchResult["artist"] = GetItemText(data, 0);
		else if (resultType == "album") {
			searchResult["type"] = GetItemText(data, 1);
			searchResult["playlistId"] = data["overlay"]["musicItemThumbnailOverlayRenderer"]["content"]["musicPlayButtonRenderer"]["playNavigationEndpoint"]["watchPlaylistEndpoint"]["playlistId"];
		}
		else if (resultType == "playlist") {
			nlohmann::json flexItem = GetFlexColumnItem(data, 1)["text"]["runs"];
			bool hasAuthor = flexItem.size() == defaultOffset + 3;
			searchResult["itemCount"] = StringUtils::Split(GetItemText(data, 1, defaultOffset + hasAuthor * 2), " ")[0];

			if (hasAuthor) searchResult["author"] = GetItemText(data, 1, defaultOffset);
			else searchResult["author"] = "";
		}
		else if (resultType == "station") {
			searchResult["videoId"] = data["navigationEndpoint"]["watchEndpoint"]["videoId"];
			searchResult["playlistId"] = data["navigationEndpoint"]["watchEndpoint"]["playlistId"];
		}
		else if (resultType == "profile") searchResult["name"] = GetItemText(data, 1, 2);
		else if (resultType == "song") searchResult["album"] = "";
		else if (resultType == "upload") {
			std::string browseId = data["navigationEndpoint"]["browseEndpoint"]["browseId"];
			if (browseId != "") {
				nlohmann::json flexItems = nlohmann::json::array();
				for (int i = 0; i < 2; i++) {
					nlohmann::json flexColumnItem = GetFlexColumnItem(data, i);
					flexItems.push_back(flexColumnItem["text"]["runs"]);
				}
				if (!flexItems[0].is_null()) {
					searchResult["videoId"] = flexItems[0][0]["navigationEndpoint"]["watchEndpoint"]["videoId"];
					searchResult["playlistId"] = flexItems[0][0]["navigationEndpoint"]["watchEndpoint"]["playlistId"];
				}
				if (!flexItems[1].is_null()) {
					nlohmann::json songRuns = ParseSongRuns(flexItems[1]);
					searchResult.merge_patch(songRuns);
				}
			} else {
				searchResult["browseId"] = browseId;
				if (StringUtils::Contains(browseId, "artist")) {
					searchResult["resultType"] = "artist";
				} else {
					nlohmann::json flexItem2 = GetFlexColumnItem(data, 1);
					nlohmann::json  flexItem2Runs = flexItem2["text"]["runs"];
					std::vector<std::string> runs;
					for (int i = 0; i < flexItem2Runs.size(); i++) {
						nlohmann::json run = flexItem2Runs[i];
						if(i % 2 == 0) runs.push_back(run["text"]);
					}

					if (runs.size() > 1) searchResult["artist"] = runs[1];
					if (runs.size() > 2) searchResult["releaseData"] = runs[2];
					searchResult["resultType"] = "album";
				}
			}
		}

		if (resultType == "song" || resultType == "video") {
			searchResult["videoId"] = data["overlay"]["musicItemThumbnailOverlayRenderer"]["content"]["musicPlayButtonRenderer"]["playNavigationEndpoint"]["watchEndpoint"]["videoId"];
			searchResult["videoType"] = videoType;
		}

		if (resultType == "song" || resultType == "video" || resultType == "album") {
			searchResult["duration"] = "";
			searchResult["year"] = "";
			nlohmann::json flexItem = GetFlexColumnItem(data, 1);
			nlohmann::json runs = flexItem["text"]["runs"];
			nlohmann::json songInfo = ParseSongRuns(runs, defaultOffset);
			searchResult.merge_patch(songInfo);
		}

		if (resultType == "album" || resultType == "playlist") {
			searchResult["browseId"] = data["navigationEndpoint"]["browseEndpoint"]["browseId"];
		}

		if (resultType == "song" || resultType == "album")
			searchResult["isExplicit"] = data.contains("badges");

		finalResults.push_back(searchResult);
	}
	
	return finalResults;
}

/*
Lyrics YTMusicAPI::GetLyrics(QString videoId, bool timestamps) {
	QString lyricsBrowseId = GetLyricsBrowseId(videoId);

	if (lyricsBrowseId.isEmpty())
		return Lyrics();

	QJsonObject context = GetContext();

	// Modify context to act as android to get timestamps
	if (timestamps) {
		QJsonObject clientContext = context["client"].toObject();
		clientContext["clientName"] = "ANDROID_MUSIC";
		clientContext["clientVersion"] = "7.21.50";
		context["client"] = clientContext;
	}

	QJsonObject body{
		{"browseId", lyricsBrowseId},
		{"context", context}
	};

	QNetworkRequest request = GetRequest("browse");

	QByteArray postData = QJsonDocument(body).toJson();
	QByteArray response = Network::Post(request, postData);
	QJsonObject json = QJsonDocument::fromJson(response).object();

	if (!response.contains("contents")) return Lyrics();

	// Look for synced lyrics
	QJsonObject data = JSONUtils::Navigate(json, { "contents", "elementRenderer", "newElement", "type", "componentType", "model", "timedLyricsModel", "lyricsData" }).toObject();
	if (timestamps && !data.isEmpty()) {
		if (!data.contains("timedLyricsData"))
			return Lyrics();

		// Setup lyrics source message
		Lyrics lyrics;
		lyrics.SourceMessage = std::format("YouTube: {}", data["sourceMessage"].toString().split("Source: ")[1].toStdString());

		// Get lyrics
		QJsonArray timedLyricsData = data["timedLyricsData"].toArray();
		std::list<Lyrics::SynchronisedLyric> lyricsList;

		bool unsyncedTimedLyrics = false;

		foreach(QJsonValue lyricsValue, timedLyricsData) {
			QJsonObject lyricsObject = lyricsValue.toObject();
			QJsonObject cueRange = lyricsObject["cueRange"].toObject();

			if (cueRange.isEmpty())
				unsyncedTimedLyrics = true;

			// Cant do QJsonValue.toInt(), times are stored as strings convert to them first
			int startMs = cueRange["startTimeMilliseconds"].toString().toInt();
			int endMs = cueRange["endTimeMilliseconds"].toString().toInt();
			std::string sentence = lyricsObject["lyricLine"].toString().toStdString();

			Lyrics::SynchronisedLyric lyric(startMs, endMs, sentence);
			lyricsList.push_back(lyric);
		}

		// If cue range is empty for any timed lyric, it will be for all of them
		// in this case loop through the timed lyrics and return as an unsynced 
		if (unsyncedTimedLyrics) {
			std::string unsyncedTimedLyricsString = "";
			foreach(Lyrics::SynchronisedLyric syncedLyric, lyricsList) {
				unsyncedTimedLyricsString += std::format("{}\n", syncedLyric.Lyric);
			}

			lyrics.Type = Lyrics::LyricsType::Unsynced;
			lyrics.UnsyncedLyrics = unsyncedTimedLyricsString;

			return lyrics;
		}

		lyrics.Type = Lyrics::LyricsType::Synced;
		lyrics.SyncedLyrics = lyricsList;

		return lyrics;
	}

	// If no synced lyrics found, look for regular lyrics
	std::string lyricsString = JSONUtils::Navigate(json, { "contents", "sectionListRenderer", "contents", 0, "musicDescriptionShelfRenderer", "description", "runs", 0, "text" }).toString().toStdString();

	if (lyricsString.empty()) return Lyrics();

	Lyrics lyrics;
	lyrics.Type = Lyrics::LyricsType::Unsynced;
	lyrics.SourceMessage = "YouTube";
	lyrics.UnsyncedLyrics = lyricsString;

	return lyrics;
}

QString YTMusicAPI::GetLyricsBrowseId(QString videoId) {
	QJsonObject body{
		{"enablePersistentPlaylistPanel", true},
		{"isAudioOnly", true},
		{"tunerSettingValue", "AUTOMIX_SETTING_NORMAL"},
		{"videoId", videoId},
		{"playlistId", QString("RDAMVM%1").arg(videoId)},
		{"watchEndpointMusicSupportedConfigs", QJsonObject {
			{"watchEndpointMusicConfig", QJsonObject {
				{"hasPersistentPlaylistPanel", true},
				{"musicVideoType", "MUSIC_VIDEO_TYPE_ATV"}
			}}
		}},
		{"context", GetContext()}
	};

	QByteArray postData = QJsonDocument(body).toJson();
	QByteArray response = Network::Post(GetRequest("next"), postData);
	QJsonObject json = QJsonDocument::fromJson(response).object();

	if (!response.contains("contents")) return "";

	QJsonObject watchNextRenderer = JSONUtils::Navigate(json, { "contents", "singleColumnMusicWatchNextResultsRenderer", "tabbedRenderer", "watchNextTabbedResultsRenderer" }).toObject();
	QString lyricsBrowseId = GetTabBrowseId(watchNextRenderer, 1);
	
	return lyricsBrowseId;
}

QString YTMusicAPI::GetTabBrowseId(QJsonObject watchNextRenderer, int tabId) {
	QJsonObject tabRenderer = JSONUtils::Navigate(watchNextRenderer, { "tabs", tabId, "tabRenderer" }).toObject();
	
	if (tabRenderer.contains("unselectable"))
		return "";

	return JSONUtils::Navigate(tabRenderer, { "endpoint", "browseEndpoint", "browseId" }).toString();
}

bool YTMusicAPI::HasPremium(QString cookies) {
	if (cookies.isEmpty())
		return false;

	QUrl url = QUrl("https://www.youtube.com/paid_memberships");
	QNetworkRequest request = QNetworkRequest(url);
	request.setRawHeader("user-agent", "Mozilla/5.0");
	request.setRawHeader("accept-language", "en-US,en;q=0.5");
	request.setRawHeader("origin", "https://www.youtube.com");

	QNetworkCookieJar* cookieJar = Network::FromNetscapeCookies(cookies);
	QByteArray response = Network::Get(request, cookieJar);
	QString responseString = QString(response);

	if (responseString.isEmpty())
		return false;

	// Returns a mix of html, js, json. Get the json that has the page layout
	QString jsonLine = responseString.split("\n")[21];
	QRegularExpression regex(R"(ytInitialData\s=\s(.*}}}}}}});)");
	QStringList matches = regex.match(jsonLine).capturedTexts();

	if (matches.count() < 2)
		return false;

	QString jsonString = matches[1];
	QJsonObject json = QJsonDocument::fromJson(jsonString.toUtf8()).object();

	QJsonArray pageSections = JSONUtils::Navigate(json, { "contents", "twoColumnBrowseResultsRenderer", "tabs", 0, "tabRenderer", "content", "sectionListRenderer", "contents" }).toArray();

	// Look for the memberships section
	QJsonArray membershipsContents = QJsonArray();
	for (QJsonValue sectionValue : pageSections) {
		QJsonObject section = sectionValue.toObject();

		// Check the section text
		QJsonArray contents = JSONUtils::Navigate(section, { "itemSectionRenderer", "contents" }).toArray();
		QString sectionText = JSONUtils::Navigate(contents, { 0, "cardItemRenderer", "headingRenderer", "cardItemTextCollectionRenderer", "textRenderers", 0, "cardItemTextRenderer", "text", "runs", 0, "text"}).toString();

		if (sectionText == "Memberships") {
			membershipsContents = contents;
			break;
		}
	}

	// No memberships found
	if (membershipsContents.isEmpty())
		return false;

	// Check each section and look for either a valid premium image alt text
	for (QJsonValue membershipValue : membershipsContents) {
		QJsonObject membership = membershipValue.toObject();

		QString imageAltText = JSONUtils::Navigate(membership, { "cardItemContainerRenderer", "baseRenderer", "cardItemRenderer", "headingRenderer", "cardItemTextWithImageRenderer", "imageRenderer", "themedImageRenderer", "imageLight", "accessibility", "accessibilityData", "label" }).toString();

		// If any valid premium alt text found, return true
		if (VALID_PREMIUM_IMAGE_ALT_TEXT.contains(imageAltText))
			return true;
	}

	return false;
}

// If video is taken down or doesnt exist it will show a "This video isn't available anymore screen"
bool YTMusicAPI::IsAgeRestricted(QString videoId) {
	// Get the song details
	QJsonObject body{
		{"playbackContext", QJsonObject{
			{"contentPlaybackContext", QJsonObject{
				{"signatureTimestamp", QDateTime::currentSecsSinceEpoch() / 86400 - 1}
			}}
		}},
		{"video_id", videoId},
		{"context", GetContext()}
	};

	QByteArray postData = QJsonDocument(body).toJson();
	QByteArray response = Network::Post(GetRequest("player"), postData);
	QJsonObject json = QJsonDocument::fromJson(response).object();

	// Get the playability status, if not returned cannot check
	if (!json.contains("playabilityStatus"))
		return false;

	QJsonObject playabiityStatus = json["playabilityStatus"].toObject();
	QString status = playabiityStatus["status"].toString();

	// No error
	if (status == "OK")
		return false;

	QString reason = playabiityStatus["reason"].toString();

	// Age restricted
	if (status == "LOGIN_REQUIRED" && reason == "Sign in to confirm your age")
		return true;

	// Other error
	return false;
}

*/