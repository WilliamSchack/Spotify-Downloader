#include "YTMusicAPI.h"

YTMusicAPI::YTMusicAPI() {
	// Network Request
	QUrl url = QUrl("https://music.youtube.com/youtubei/v1/search/?alt=json");
	_request = QNetworkRequest(url);
	_request.setRawHeader("user-agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:88.0) Gecko/20100101 Firefox/88.0");
	_request.setRawHeader("accept", "*/*");
	_request.setRawHeader("acceps-encoding", "gzip, deflate");
	_request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	_request.setRawHeader("content-encoding", "gzip");
	_request.setRawHeader("origin", "https://music.youtube.com/youtubei/v1/");
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
	if (filter == "songs") searchParams.append("II"); //  Filter
	else if (filter == "videos") searchParams.append("IQ"); // ^
	searchParams.append("AUICCAFqDBAOEAoQAxAEEAkQBQ%3D%3D"); // Ignore Spelling

	QJsonObject body {
		{"query", query},
		{"params", searchParams},
		{"context", GetContext()}
	};

	QNetworkAccessManager* manager = new QNetworkAccessManager();
	QByteArray postData = QJsonDocument(body).toJson();
	QByteArray response = Network::Post(_request, postData);
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

				QNetworkRequest continuationRequest = _request;
				continuationRequest.setUrl(_request.url().toString() + additionalParams);
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
		else if (resultType == "album") searchResult["type"] = GetItemText(data, 1);
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

		finalResults.append(searchResult);
	}
	
	return finalResults;
}