#include "YTMusicAPI.h"

bool YTMusicAPI::CheckConnection() {
	QUrl url = QUrl("https://music.youtube.com/youtubei/v1/");
	return Network::Ping(url);
}

QNetworkRequest YTMusicAPI::GetRequest(QString endpoint) {
	QUrl url = QUrl(QString("https://music.youtube.com/youtubei/v1/%1/?alt=json").arg(endpoint));
	QNetworkRequest request = QNetworkRequest(url);
	request.setRawHeader("user-agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:88.0) Gecko/20100101 Firefox/88.0");
	request.setRawHeader("accept", "*/*");
	request.setRawHeader("acceps-encoding", "gzip, deflate");
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
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

	QNetworkAccessManager* manager = new QNetworkAccessManager();
	QByteArray postData = QJsonDocument(body).toJson();
	QByteArray response = Network::Post(GetRequest("search"), postData);
	QJsonObject json = QJsonDocument::fromJson(response).object();

	// Parse page into results
	QJsonArray searchResults = QJsonArray();

	// No results
	if (!json.contains("contents")) return searchResults;

	QJsonObject results;
	if (json["contents"].toObject().contains("tabbedSearchResultsRenderer")) {
		results = json["contents"].toObject()["tabbedSearchResultsRenderer"].toObject()["tabs"].toArray()[0].toObject()["tabRenderer"].toObject()["content"].toObject();
	} else {
		results = json["contents"].toObject();
	}

	QJsonArray contents = results["sectionListRenderer"].toObject()["contents"].toArray();

	// No results
	//if (contents.count() == 1 && contents.contains("itemSectionRenderer")) return searchResults;

	foreach(QJsonValue val, contents) {
		QJsonObject result = val.toObject();

		QString type;
		QString catagory;

		if (result.contains("musicCardShelfRenderer")) {
			QJsonObject data = result["musicCardShelfRenderer"].toObject();

			QStringList resultTypes = QStringList{ "artist", "playlist", "song", "video", "station", "profile" };
			QString subtitle = data["subtitle"].toObject()["runs"].toArray()[0].toObject()["text"].toString();
			QString resultType = subtitle.toLower();
			if (!resultTypes.contains(resultType)) resultType = "album";
			QJsonObject topResult = QJsonObject();
			topResult["resultType"] = resultType;

			QString catagory = data["header"].toObject()["musicCardShelfHeaderBasicRenderer"].toObject()["title"].toObject()["runs"].toArray()[0].toObject()["text"].toString();
			topResult["catagory"] = catagory;

			if (resultType == "song" || resultType == "video") {
				QJsonObject onTap = data["onTap"].toObject();
				if (!onTap.isEmpty()) {
					topResult["videoId"] = onTap["watchEndpoint"].toObject()["videoId"].toString();
					topResult["videoType"] = onTap["watchEndpoint"].toObject()["watchEndpointMusicSupportedConfigs"].toObject()["watchEndpointMusicConfig"].toObject()["musicVideoType"].toString();
				}
			}

			if (resultType == "song" || resultType == "video" || resultType == "album") {
				topResult["title"] = data["title"].toObject()["runs"].toArray()[0].toObject()["text"].toString();
				QJsonArray runs = data["subtitle"].toObject()["runs"].toArray();
				QJsonObject songInfo = ParseSongRuns(runs, 2);
				topResult = JSONUtils::Merge(topResult, songInfo);
			}

			searchResults.append(topResult);

			if (contents == result["musicCardShelfRenderer"].toObject()["contents"].toArray()) {
				contents = result["musicCardShelfRenderer"].toObject()["contents"].toArray();
				catagory = "";
				if (contents[0].toObject().contains("messageRenderer")) {
					contents.pop_front();
					catagory = contents[0].toObject()["messageRenderer"].toObject()["text"].toObject()["runs"].toArray()[0].toObject()["text"].toString();
				}
				//type = filter.removeLast();
			} else {
				contents = result["musicCardShelfRenderer"].toObject()["contents"].toArray();
				continue;
			}

		} else if (result.contains("musicShelfRenderer")) {
			contents = result["musicShelfRenderer"].toObject()["contents"].toArray();
			catagory = result["musicShelfRenderer"].toObject()["title"].toObject()["runs"].toArray()[0].toObject()["text"].toString();
			type = filter.removeLast().toLower();
		} else {
			continue;
		}

		// Limit search results
		if (contents.count() > limit) {
			while (contents.count() > limit) contents.removeLast();
		}

		QJsonArray currentSearchResults = ParseSearchResults(contents, type, catagory);
		searchResults = JSONUtils::Extend(searchResults, currentSearchResults);

		if (filter != "") {
			QJsonObject continuationResults = result["musicShelfRenderer"].toObject();
			int currentLimit = limit - searchResults.count();

			QJsonArray items = QJsonArray();
			while (continuationResults.contains("continuations") && items.count() < currentLimit) {
				QString ctoken = continuationResults["continuations"].toArray()[0].toObject()["nextContinuationData"].toObject()["continuation"].toString();
				QString additionalParams = QString("&ctoken=%1&continuation=%1").arg(ctoken);

				QNetworkRequest continuationRequest = GetRequest("search");
				continuationRequest.setUrl(continuationRequest.url().toString() + additionalParams);
				QByteArray continuationResponse = Network::Post(continuationRequest, postData);
				QJsonObject continuationJson = QJsonDocument::fromJson(continuationResponse).object();

				if (continuationJson.contains("continuationContents")) continuationResults = continuationJson["continuationContents"].toObject()["musicShelfContinuation"].toObject();
				else break;

				QJsonArray continuationContents = QJsonArray();
				if (continuationResults.contains("contents")) continuationContents = ParseSearchResults(continuationResults["contents"].toArray(), type, catagory);
				else if (continuationResults.contains("items")) continuationContents = ParseSearchResults(continuationResults["items"].toArray(), type, catagory);

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

	QNetworkAccessManager* manager = new QNetworkAccessManager();
	QByteArray postData = QJsonDocument(body).toJson();
	QByteArray response = Network::Post(GetRequest("browse"), postData);
	QJsonObject json = QJsonDocument::fromJson(response).object();

	QJsonObject album = ParseAlbumHeader(json);

	QJsonObject results = json["contents"].toObject()["twoColumnBrowseResultsRenderer"].toObject()["secondaryContents"].toObject()
		["sectionListRenderer"].toObject()["contents"].toArray()[0].toObject()["musicShelfRenderer"].toObject();
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
	QJsonObject header = response["contents"].toObject()["twoColumnBrowseResultsRenderer"].toObject()
		["tabs"].toArray()[0].toObject()["tabRenderer"].toObject()["content"].toObject()
		["sectionListRenderer"].toObject()["contents"].toArray()[0].toObject()["musicResponsiveHeaderRenderer"].toObject();

	QJsonObject album {
		{"title", header["title"].toObject()["runs"].toArray()[0].toObject()["text"].toString()},
		{"type", header["subtitle"].toObject()["runs"].toArray()[0].toObject()["text"].toString()}
	};

	if (header.contains("description"))
		album["description"] = header["description"].toObject()["musicDescriptionShelfRenderer"].toObject()["description"].toObject()["runs"].toArray()[0].toObject()["text"].toString();
	
	QJsonObject albumInfo = ParseSongRuns(header["subtitle"].toObject()["runs"].toArray()); // Artists not included, not required for the program
	if (!header["straplineTextOne"].toObject().isEmpty()) { // If this does not pass, artists will be found for tracks individually, only happens for very few albums
		albumInfo["artists"] = QJsonArray{
			QJsonObject {
				{"name", header["straplineTextOne"].toObject()["runs"].toArray()[0].toObject()["text"].toString()},
				{"id", header["straplineTextOne"].toObject()["runs"].toArray()[0].toObject()["navigationEndpoint"].toObject()["browseEndpoint"].toObject()["browseId"].toString()}
			}
		};
	}
	album = JSONUtils::Merge(album, albumInfo);

	if (header["secondSubtitle"].toObject()["runs"].toArray().count() > 1) {
		album["trackCount"] = header["secondSubtitle"].toObject()["runs"].toArray()[0].toObject()["text"].toString().split(" ")[0].toInt(); // Output is -- Songs, remove " songs" and convert to int
		album["duration"] = header["secondSubtitle"].toObject()["runs"].toArray()[2].toObject()["text"].toString();
	} else {
		album["duration"] = header["secondSubtitle"].toObject()["runs"].toArray()[0].toObject()["text"].toString();
	}

	QJsonArray buttons = header["buttons"].toArray();
	album["audioPlaylistId"] = buttons[1].toObject()["musicPlayButtonRenderer"].toObject()["playNavigationEndpoint"].toObject()["watchEndpoint"].toObject()["playlistId"].toString();

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
					QJsonObject menuService = item["menuServiceItemRenderer"].toObject()["menuServiceItemRenderer"].toObject();
					if (menuService.contains("playlistEditEndpoint")) {
						song["setVideoId"] = menuService["playlistEditEndpoint"].toObject()["actions"].toArray()[0].toObject()["setVideoId"].toString();
						song["videoId"] = menuService["playlistEditEndpoint"].toObject()["actions"].toArray()[0].toObject()["removedVideoId"].toString();
					}
				}
			}
		}

		if (JSONUtils::BranchExists(data, { "overlay", "musicItemThumbnailOverlayRenderer", "content", "musicPlayButtonRenderer" })) {
			song["videoId"] = data["overlay"].toObject()["musicItemThumbnailOverlayRenderer"].toObject()["content"].toObject()["musicPlayButtonRenderer"].toObject()
								["playNavigationEndpoint"].toObject()["watchEndpoint"].toObject()["videoId"].toString();
		}

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

			if (!JSONUtils::BranchExists(flexColumnItem, { "text", "runs", "0", "navigationEndpoint" })) {
				if (JSONUtils::BranchExists(flexColumnItem, { "text", "runs", "0", "text" })) {
					unrecognisedIndex = unrecognisedIndex == -1 ? index : unrecognisedIndex;
				}

				continue;
			}

			QJsonObject navigationEndpoint = flexColumnItem["text"].toObject()["runs"].toArray()[0].toObject()["navigationEndpoint"].toObject();

			if (navigationEndpoint.contains("watchEndpoint")) {
				titleIndex = index;
			} else if (navigationEndpoint.contains("browseEndpoint")) {
				QString pageType = navigationEndpoint["browseEndpoint"].toObject()["browseEndpointContectSupportedConfigs"].toObject()["browseEndpointContextMusicConfig"].toObject()["pageType"].toString();

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

		if (data.contains("fixedColumns")) {
			QString duration = "";
			QJsonObject fixedColumnItemText = GetFixedColumnItem(data, 0)["text"].toObject();
			if (fixedColumnItemText.contains("simpleText"))
				duration = fixedColumnItemText["simpleText"].toString();
			else
				duration = fixedColumnItemText["runs"].toArray()[0].toObject()["text"].toString();

			song["duration"] = duration;
			song["durationSeconds"] = TimeToSeconds(duration);
		}

		song["videoType"] = "";
		if (data.contains("menu")) {
			song["videoType"] = data["menu"].toObject()["menuRenderer"].toObject()["items"].toArray()[0].toObject()["menuNavigationItemRenderer"].toObject()
				["navigationEndpoint"].toObject()["watchEndpoint"].toObject()["watchEndpointMusicSupportedConfigs"].toObject()["watchEndpointMusicConfig"].toObject()["musicVideoType"].toString();
		}

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
				{"id", run["navigationEndpoint"].toObject()["browseEndpoint"].toObject()["browseId"].toString()}
			};

			if (item.contains("id") && item["id"].toString().startsWith("MPRE") || item["id"].toObject().contains("release_detail")) {
				parsed["album"] = item;
			}
			else {
				artists.append(item);
			}
		}
		else {
			if (std::regex_match(text.toStdString(), std::regex(R"(^\d([^ ])* [^ ]*$)")) && i > 2) {
				parsed["views"] = text.split(" ")[0];
			}
			else if (std::regex_match(text.toStdString(), std::regex(R"(^(\d+:)*\d+:\d+$)"))) {
				parsed["duration"] = text;
				parsed["durationSeconds"] = TimeToSeconds(text);
			}
			else if (std::regex_match(text.toStdString(), std::regex(R"(^\d{4}$)"))) {
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
		!flexColumns[index].toObject()["musicResponsiveListItemFlexColumnRenderer"].toObject().contains("text") ||
		!flexColumns[index].toObject()["musicResponsiveListItemFlexColumnRenderer"].toObject()["text"].toObject().contains("runs")) {
		return QJsonObject();
	}

	return flexColumns[index].toObject()["musicResponsiveListItemFlexColumnRenderer"].toObject();
}

QJsonObject YTMusicAPI::GetFixedColumnItem(QJsonObject item, int index) {
	if (!item["fixedColumns"].toArray()[index].toObject()["musicResponsiveListItemFixedColumnRenderer"].toObject().contains("text") ||
		!item["fixedColumns"].toArray()[index].toObject()["musicResponsiveListItemFixedColumnRenderer"].toObject()["text"].toObject().contains("runs")) {
		
		return QJsonObject();
	}

	return item["fixedColumns"].toArray()[index].toObject()["musicResponsiveListItemFixedColumnRenderer"].toObject();
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
				{ "name", runs[i * 2].toObject()["text"].toString() },
				{ "id", runs[i * 2].toObject()["navigationEndpoint"].toObject()["browseEndpoint"].toObject()["browseId"].toString() }
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
	
	QString browseId = flexItem["text"].toObject()["runs"].toArray()[0].toObject()
						["navigationEndpoint"].toObject()["browseEndpoint"].toObject()["browseId"].toString();

	return QJsonObject {
		{ "name", GetItemText(data, index) },
		{ "id", browseId }
	};
}

QJsonArray YTMusicAPI::ParseSearchResults(QJsonArray results, QString resultType, QString catagory) {
	QJsonArray finalResults = QJsonArray();

	int defaultOffset = (resultType == "" ? true : false) * 2;

	foreach(QJsonValue val, results) {
		QJsonObject data = val.toObject()["musicResponsiveListItemRenderer"].toObject();

		QJsonObject searchResult {
			{"catagory", catagory}
		};

		QString videoType = data["overlay"].toObject()["musicItemThumbnailOverlayRenderer"].toObject()["content"].toObject()["musicPlayButtonRenderer"].toObject()["playNavigationEndpoint"].toObject()
			["watchEndpoint"].toObject()["watchEndpointMusicSupportedConfigs"].toObject()["watchEndpointMusicConfig"].toObject()["musicVideoType"].toString();
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
			searchResult["playlistId"] = data["overlay"].toObject()["musicItemThumbnailOverlayRenderer"].toObject()["content"].toObject()["musicPlayButtonRenderer"].toObject()["playNavigationEndpoint"].toObject()
				["watchPlaylistEndpoint"].toObject()["playlistId"].toString();
		}
		else if (resultType == "playlist") {
			QJsonArray flexItem = GetFlexColumnItem(data, 1)["text"].toObject()["runs"].toArray();
			bool hasAuthor = flexItem.count() == defaultOffset + 3;
			searchResult["itemCount"] = GetItemText(data, 1, defaultOffset + hasAuthor * 2).split(" ")[0];
			if (hasAuthor) searchResult["author"] = GetItemText(data, 1, defaultOffset);
			else searchResult["author"] = "";
		}
		else if (resultType == "station") {
			searchResult["videoId"] = data["navigationEndpoint"].toObject()["watchEndpoint"].toObject()["videoId"].toString();
			searchResult["playlistId"] = data["navigationEndpoint"].toObject()["watchEndpoint"].toObject()["playlistId"].toString();
		}
		else if (resultType == "profile") searchResult["name"] = GetItemText(data, 1, 2);
		else if (resultType == "song") searchResult["album"] = "";
		else if (resultType == "upload") {
			QString browseId = data["navigationEndpoint"].toObject()["browseEndpoint"].toObject()["browseId"].toString();
			if (browseId != "") {
				QJsonArray flexItems = QJsonArray();
				for (int i = 0; i < 2; i++) {
					QJsonObject flexColumnItem = GetFlexColumnItem(data, i);
					flexItems.append(flexColumnItem["text"].toObject()["runs"].toArray());
				}
				if (!flexItems[0].isNull()) {
					searchResult["videoId"] = flexItems[0].toArray()[0].toObject()["navigationEndpoint"].toObject()["watchEndpoint"].toObject()["videoId"].toString();
					searchResult["playlistId"] = flexItems[0].toArray()[0].toObject()["navigationEndpoint"].toObject()["watchEndpoint"].toObject()["playlistId"].toString();
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
					QJsonArray flexItem2Runs = flexItem2["text"].toObject()["runs"].toArray();
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
			searchResult["videoId"] = data["overlay"].toObject()["musicItemThumbnailOverlayRenderer"].toObject()["content"].toObject()["musicPlayButtonRenderer"].toObject()
				["playNavigationEndpoint"].toObject()["watchEndpoint"].toObject()["videoId"].toString();
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
			searchResult["browseId"] = data["navigationEndpoint"].toObject()["browseEndpoint"].toObject()["browseId"].toString();
		}

		finalResults.append(searchResult);
	}
	
	return finalResults;
}