#include "YTMusicAPI.h"

bool YTMusicAPI::CheckConnection() {
	QUrl url = QUrl("https://music.youtube.com/youtubei/v1/");
	return Network::Ping(url);
}

QNetworkRequest YTMusicAPI::GetRequest(QString endpoint) {
	QUrl url = QUrl(QString("https://music.youtube.com/youtubei/v1/%1/?alt=json").arg(endpoint));
	QNetworkRequest request = QNetworkRequest(url);
	request.setRawHeader("user-agent", "Mozilla/5.0");
	request.setRawHeader("accept", "*/*");
	request.setRawHeader("accept-encoding", "gzip, deflate");
	request.setRawHeader("content-type", "application/json");
	request.setRawHeader("content-encoding", "gzip");
	request.setRawHeader("origin", "https://music.youtube.com/youtubei/v1/");

	return request;
}

QJsonObject YTMusicAPI::GetContext() {
	std::time_t rawTime;
	struct tm* timeInfo;
	char buffer[80];
	std::time(&rawTime);
	timeInfo = std::localtime(&rawTime);
	std::strftime(buffer, sizeof(buffer), "%Y%m%d", timeInfo);
	QString clientVersion = QString("1.%1.01.00").arg(buffer);

	return QJsonObject{
		{"client", QJsonObject{
			{"clientName", "WEB_REMIX"},
			{"clientVersion", clientVersion}
		}},
		{"user", QJsonObject{}}
	};
}

QJsonArray YTMusicAPI::Search(QString query, QString filter, int limit) {
	// Get web page
	QString searchParams = "EgWKAQ"; // Param 1
	if (filter == "songs") searchParams.append("II"); //  Songs Filter
	else if (filter == "videos") searchParams.append("IQ"); // Videos Filter
	else if (filter == "albums") searchParams.append("IY"); // Albums Filter
	searchParams.append("AUICCAFqDBAOEAoQAxAEEAkQBQ%3D%3D"); // Ignore Spelling

	QJsonObject body {
		{"query", query},
		{"params", searchParams},
		{"context", GetContext()}
	};

	QByteArray postData = QJsonDocument(body).toJson();
	QByteArray response = Network::Post(GetRequest("search"), postData);
	QJsonObject json = QJsonDocument::fromJson(response).object();

	// Parse page into results
	QJsonArray searchResults = QJsonArray();

	// No results
	if (!json.contains("contents")) return searchResults;

	QJsonObject results;
	if (json["contents"].toObject().contains("tabbedSearchResultsRenderer")) {
		results = JSONUtils::Navigate(json, { "contents", "tabbedSearchResultsRenderer", "tabs", 0, "tabRenderer", "content" }).toObject();
	} else {
		results = json["contents"].toObject();
	}

	QJsonArray contents = JSONUtils::Navigate(results, { "sectionListRenderer", "contents" }).toArray();

	foreach(QJsonValue val, contents) {
		QJsonObject result = val.toObject();

		QString type;
		QString category;

		if (result.contains("musicCardShelfRenderer")) {
			QJsonObject data = result["musicCardShelfRenderer"].toObject();

			QStringList resultTypes = QStringList{ "artist", "playlist", "song", "video", "station", "profile" };
			QString subtitle = JSONUtils::Navigate(data, { "subtitle", "runs", 0, "text" }).toString();
			QString resultType = subtitle.toLower();
			if (!resultTypes.contains(resultType)) resultType = "album";
			QJsonObject topResult = QJsonObject();
			topResult["resultType"] = resultType;

			QString category = JSONUtils::Navigate(data, { "header", "musicCardShelfHeaderBasicRenderer", "title", "runs", 0, "text" }).toString();
			topResult["category"] = category;

			if (resultType == "song" || resultType == "video") {
				QJsonObject onTap = data["onTap"].toObject();
				if (!onTap.isEmpty()) {
					topResult["videoId"] = JSONUtils::Navigate(onTap, { "watchEndpoint", "videoId" }).toString();
					topResult["videoType"] = JSONUtils::Navigate(onTap, { "watchEndpoint", "watchEndpointMusicSupportedConfigs", "watchEndpointMusicConfig", "musicVideoType" }).toString();
				}
			}

			if (resultType == "song" || resultType == "video" || resultType == "album") {
				topResult["title"] = JSONUtils::Navigate(data, { "title", "runs", 0, "text" }).toString();
				QJsonArray runs = JSONUtils::Navigate(data, { "subtitle", "runs" }).toArray();
				QJsonObject songInfo = ParseSongRuns(runs, 2);
				topResult = JSONUtils::Merge(topResult, songInfo);
			}

			searchResults.append(topResult);

			if (contents == JSONUtils::Navigate(result, { "musicCardShelfRenderer", "contents" }).toArray()) {
				contents = JSONUtils::Navigate(result, { "musicCardShelfRenderer", "contents" }).toArray();
				category = "";
				if (contents[0].toObject().contains("messageRenderer")) {
					contents.pop_front();
					category = JSONUtils::Navigate(contents, { 0, "messageRenderer", "text", "runs", 0, "text" }).toString();
				}
				//type = filter.removeLast();
			} else {
				contents = JSONUtils::Navigate(result, { "musicCardShelfRenderer", "contents" }).toArray();
				continue;
			}

		} else if (result.contains("musicShelfRenderer")) {
			contents = JSONUtils::Navigate(result, { "musicShelfRenderer", "contents" }).toArray();
			category = JSONUtils::Navigate(result, { "musicShelfRenderer", "title", "runs", 0, "text" }).toString();
			type = filter.removeLast().toLower();
		} else {
			continue;
		}

		// Limit search results
		if (contents.count() > limit) {
			while (contents.count() > limit) contents.removeLast();
		}

		QJsonArray currentSearchResults = ParseSearchResults(contents, type, category);
		searchResults = JSONUtils::Extend(searchResults, currentSearchResults);

		if (filter != "") {
			QJsonObject continuationResults = result["musicShelfRenderer"].toObject();
			int currentLimit = limit - searchResults.count();

			QJsonArray items = QJsonArray();
			while (continuationResults.contains("continuations") && items.count() < currentLimit) {
				QString ctoken = JSONUtils::Navigate(continuationResults, { "continuations", 0, "nextContinuationData", "continuation" }).toString();
				QString additionalParams = QString("&ctoken=%1&continuation=%1").arg(ctoken);

				QNetworkRequest continuationRequest = GetRequest("search");
				continuationRequest.setUrl(continuationRequest.url().toString() + additionalParams);
				QByteArray continuationResponse = Network::Post(continuationRequest, postData);
				QJsonObject continuationJson = QJsonDocument::fromJson(continuationResponse).object();

				if (continuationJson.contains("continuationContents"))
					continuationResults = JSONUtils::Navigate(continuationJson, { "continuationContents", "musicShelfContinuation" }).toObject();
				else
					break;

				QJsonArray continuationContents = QJsonArray();
				if (continuationResults.contains("contents"))
					continuationContents = ParseSearchResults(continuationResults["contents"].toArray(), type, category);
				else if (continuationResults.contains("items"))
					continuationContents = ParseSearchResults(continuationResults["items"].toArray(), type, category);

				if (continuationContents.count() == 0) break;

				items = JSONUtils::Extend(items, continuationContents);
			}

			searchResults = JSONUtils::Extend(searchResults, items);
		}
	}

	return searchResults;
}

QJsonObject YTMusicAPI::GetAlbum(QString browseId) {
	if (!browseId.startsWith("MPRE")) return QJsonObject();

	QJsonObject body {
		{"browseId", browseId},
		{"context", GetContext()}
	};

	QByteArray postData = QJsonDocument(body).toJson();
	QByteArray response = Network::Post(GetRequest("browse"), postData);
	QJsonObject json = QJsonDocument::fromJson(response).object();

	if (json.isEmpty()) return QJsonObject();

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

QJsonObject YTMusicAPI::ParseSongRuns(QJsonArray runs, int offset) {
	QJsonObject parsed;
	QJsonArray artists = {};
	for (int i = offset; i < runs.count(); i++) {
		if (i % 2) continue;
		QJsonObject run = runs[i].toObject();

		QString text = run["text"].toString();
		if (run.contains("navigationEndpoint")) {
			QJsonObject item{
				{"name", text},
				{"id", JSONUtils::Navigate(run, {"navigationEndpoint", "browseEndpoint", "browseId"}).toString()}
			};

			if (item.contains("id") && item["id"].toString().startsWith("MPRE") || item["id"].toObject().contains("release_detail")) {
				parsed["album"] = item;
			}
			else {
				artists.append(item);
			}
		}
		else {
			if (std::regex_match(text.toUtf8().constData(), std::regex(R"(^\d([^ ])* [^ ]*$)")) && i > 2) {
				parsed["views"] = text.split(" ")[0];
			}
			else if (std::regex_match(text.toUtf8().constData(), std::regex(R"(^(\d+:)*\d+:\d+$)"))) {
				parsed["duration"] = text;
				parsed["durationSeconds"] = TimeToSeconds(text);
			}
			else if (std::regex_match(text.toUtf8().constData(), std::regex(R"(^\d{4}$)"))) {
				parsed["year"] = text;
			}
			else {
				artists.append(QJsonObject{
					{"name", text},
					{"id", ""}
				});
			}
		}
	}
	parsed["artists"] = artists;
	return parsed;
}

int YTMusicAPI::TimeToSeconds(QString time) {
	QStringList seperated = time.split(":");
	int seconds = 0;
	switch (seperated.count()) {
		case 1:
			seconds = seperated[0].toInt();
			break;
		case 2:
			seconds = seperated[0].toInt() * 60 + seperated[1].toInt();
			break;
		case 3:
			seconds = seperated[0].toInt() * 3600 + seperated[1].toInt() * 60 + seperated[2].toInt();
			break;
		case 4:
			seconds = seperated[0].toInt() * 86400 + seperated[1].toInt() * 3600 + seperated[2].toInt() * 60 + seperated[3].toInt();
			break;
	}

	return seconds;
}

QString YTMusicAPI::GetItemText(QJsonObject item, int index, int runIndex) {
	QJsonObject column = GetFlexColumnItem(item, index);
	if (column.isEmpty()) return "";

	return column["text"].toObject()["runs"].toArray()[runIndex].toObject()["text"].toString();
}

QJsonObject YTMusicAPI::GetFlexColumnItem(QJsonObject item, int index) {
	QJsonArray flexColumns = item["flexColumns"].toArray();
	if (flexColumns.count() <= index ||
		!JSONUtils::Navigate(flexColumns, { index, "musicResponsiveListItemFlexColumnRenderer" }).toObject().contains("text") ||
		!JSONUtils::Navigate(flexColumns, { index, "musicResponsiveListItemFlexColumnRenderer", "text" }).toObject().contains("runs")) {
		return QJsonObject();
	}

	return JSONUtils::Navigate(flexColumns, { index, "musicResponsiveListItemFlexColumnRenderer" }).toObject();
}

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
	else {
		QJsonArray runs = flexItem["text"].toObject()["runs"].toArray();

		QJsonArray artists = QJsonArray();
		for (int i = 0; i < int(runs.count() / 2 + 1); i++) {
			QJsonObject artist{
				{ "name", JSONUtils::Navigate(runs, { i * 2, "text" }).toString() },
				{ "id", JSONUtils::Navigate(runs, { i * 2, "navigationEndpoint", "browseEndpoint", "browseId" }).toString()}
			};
			artists.append(artist);
		}

		return artists;
	}
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

QJsonArray YTMusicAPI::ParseSearchResults(QJsonArray results, QString resultType, QString category) {
	QJsonArray finalResults = QJsonArray();

	int defaultOffset = (resultType == "" ? true : false) * 2;

	foreach(QJsonValue val, results) {
		QJsonObject data = val.toObject()["musicResponsiveListItemRenderer"].toObject();

		QJsonObject searchResult {
			{"category", category}
		};

		QString videoType = JSONUtils::Navigate(data, { "overlay", "musicItemThumbnailOverlayRenderer", "content", "musicPlayButtonRenderer", "playNavigationEndpoint", "watchEndpoint", "watchEndpointMusicSupportedConfigs", "watchEndpointMusicConfig", "musicVideoType" }).toString();
		if (resultType == "" && videoType != "") {
			if (videoType == "MUSIC_VIDEO_TYPE_ATV") resultType = "song";
			else resultType = "video";
		}

		if (resultType == "") {
			QStringList resultTypes = QStringList{ "artist", "playlist", "song", "video", "station", "profile" };
			QString resultTypeLocal = GetItemText(data, 1).toLower();
			if (!resultTypes.contains(resultTypeLocal)) {
				resultType = "album";
			} else {
				resultType = resultTypeLocal;
			}
		}
		searchResult["resultType"] = resultType;

		if (resultType != "artist") {
			searchResult["title"] = GetItemText(data, 0);
		}

		if (resultType == "artist") searchResult["artist"] = GetItemText(data, 0);
		else if (resultType == "album") {
			searchResult["type"] = GetItemText(data, 1);
			searchResult["playlistId"] = JSONUtils::Navigate(data, { "overlay", "musicItemThumbnailOverlayRenderer", "content", "musicPlayButtonRenderer", "playNavigationEndpoint", "watchPlaylistEndpoint", "playlistId" }).toString();
		}
		else if (resultType == "playlist") {
			QJsonArray flexItem = JSONUtils::Navigate(GetFlexColumnItem(data, 1), { "text", "runs" }).toArray();
			bool hasAuthor = flexItem.count() == defaultOffset + 3;
			searchResult["itemCount"] = GetItemText(data, 1, defaultOffset + hasAuthor * 2).split(" ")[0];
			if (hasAuthor) searchResult["author"] = GetItemText(data, 1, defaultOffset);
			else searchResult["author"] = "";
		}
		else if (resultType == "station") {
			searchResult["videoId"] = JSONUtils::Navigate(data, { "navigationEndpoint", "watchEndpoint", "videoId" }).toString();
			searchResult["playlistId"] = JSONUtils::Navigate(data, { "navigationEndpoint", "watchEndpoint", "playlistId" }).toString();
		}
		else if (resultType == "profile") searchResult["name"] = GetItemText(data, 1, 2);
		else if (resultType == "song") searchResult["album"] = "";
		else if (resultType == "upload") {
			QString browseId = JSONUtils::Navigate(data, { "navigationEndpoint", "browseEndpoint", "browseId" }).toString();
			if (browseId != "") {
				QJsonArray flexItems = QJsonArray();
				for (int i = 0; i < 2; i++) {
					QJsonObject flexColumnItem = GetFlexColumnItem(data, i);
					flexItems.append(JSONUtils::Navigate(flexColumnItem, {"text", "runs"}).toArray());
				}
				if (!flexItems[0].isNull()) {
					qDebug() << "VIDEO ID";
					qDebug() << flexItems;
					searchResult["videoId"] = JSONUtils::Navigate(flexItems, { 0, 0, "navigationEndpoint", "watchEndpoint", "videoId"}).toString();
					searchResult["playlistId"] = JSONUtils::Navigate(flexItems, { 0, 0, "navigationEndpoint", "watchEndpoint", "playlistId"}).toString();
				}
				if (!flexItems[1].isNull()) {
					QJsonObject songRuns = ParseSongRuns(flexItems[1].toArray());
					searchResult = JSONUtils::Merge(searchResult, songRuns);
				}
			} else {
				searchResult["browseId"] = browseId;
				if (browseId.contains("artist")) {
					searchResult["resultType"] = "artist";
				} else {
					QJsonObject flexItem2 = GetFlexColumnItem(data, 1);
					QJsonArray flexItem2Runs = JSONUtils::Navigate(flexItem2, { "text", "runs" }).toArray();
					QStringList runs = QStringList();
					for (int i = 0; i < flexItem2Runs.count(); i++) {
						QJsonObject run = flexItem2Runs[i].toObject();
						if(i % 2 == 0) runs.append(run["text"].toString());
					}

					if (runs.count() > 1) searchResult["artist"] = runs[1];
					if (runs.count() > 2) searchResult["releaseData"] = runs[2];
					searchResult["resultType"] = "album";
				}
			}
		}

		if (resultType == "song" || resultType == "video") {
			searchResult["videoId"] = JSONUtils::Navigate(data, { "overlay", "musicItemThumbnailOverlayRenderer", "content", "musicPlayButtonRenderer", "playNavigationEndpoint", "watchEndpoint", "videoId"}).toString();
			searchResult["videoType"] = videoType;
		}

		if (resultType == "song" || resultType == "video" || resultType == "album") {
			searchResult["duration"] = "";
			searchResult["year"] = "";
			QJsonObject flexItem = GetFlexColumnItem(data, 1);
			QJsonArray runs = flexItem["text"].toObject()["runs"].toArray();
			QJsonObject songInfo = ParseSongRuns(runs, defaultOffset);
			searchResult = JSONUtils::Merge(searchResult, songInfo);
		}

		if (resultType == "album" || resultType == "playlist") {
			searchResult["browseId"] = JSONUtils::Navigate(data, { "navigationEndpoint", "browseEndpoint", "browseId" }).toString();
		}

		if (resultType == "song" || resultType == "album")
			searchResult["isExplicit"] = data.contains("badges");

		finalResults.append(searchResult);
	}
	
	return finalResults;
}

Lyrics YTMusicAPI::GetLyrics(QString videoId, bool timestamps) {
	QString lyricsBrowseId = GetLyricsBrowseId(videoId);

	if (lyricsBrowseId.isEmpty())
		return QJsonObject();

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

	// Look for synced lyrics
	QJsonObject data = JSONUtils::Navigate(json, { "contents", "elementRenderer", "newElement", "type", "componentType", "model", "timedLyricsModel", "lyricsData" }).toObject();
	if (timestamps && !data.isEmpty()) {
		if (!data.contains("timedLyricsData"))
			return Lyrics();

		QJsonArray timedLyricsData = data["timedLyricsData"].toArray();
		std::list<Lyrics::SynchronisedLyric> lyricsList;

		foreach(QJsonValue lyricsValue, timedLyricsData) {
			QJsonObject lyricsObject = lyricsValue.toObject();
			QJsonObject cueRange = lyricsObject["cueRange"].toObject();

			int startMs = cueRange["startTimeMilliseconds"].toInt();
			int endMs = cueRange["endTimeMilliseconds"].toInt();
			std::string sentence = lyricsObject["lyricLine"].toString().toStdString();

			Lyrics::SynchronisedLyric lyric(startMs, endMs, sentence);
			lyricsList.push_back(lyric);
		}

		Lyrics lyrics;
		lyrics.Type = Lyrics::LyricsType::Synced;
		lyrics.SyncedLyrics = lyricsList;

		return lyrics;
	}

	// If no synced lyrics found, look for regular lyrics
	std::string lyricsString = JSONUtils::Navigate(json, { "contents", "sectionListRenderer", "contents", 0, "musicDescriptionShelfRenderer", "description", "runs", 0, "text" }).toString().toStdString();

	if (lyricsString.empty())
		return Lyrics();

	Lyrics lyrics;
	lyrics.Type = Lyrics::LyricsType::Unsynced;
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