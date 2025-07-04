#include "Song.h"

Song::Song(QJsonObject song, QJsonObject album, QString ytdlpPath, QString ffmpegPath, Codec::Extension codec, const SpotifyDownloader* main) {
	_main = main;

	_ytdlpPath = ytdlpPath;
	_ffmpegPath = ffmpegPath;

	// Get universal details
	Title = song["name"].toString();
	Time = song["duration_ms"].toDouble() / 1000;
	SpotifyId = song["id"].toString();
	IsExplicit = song["explicit"].toBool();
	InPlaylist = song.contains("playlist_track_number");
	PlaylistTrackNumber = song["playlist_track_number"].toInt();

	// Handle episode, details are different (If song is episode and contains show object or album object that is a show
	if (song["type"].toString() == "episode" && (song.contains("show") || (song.contains("album") && song["album"].toObject()["type"].toString() == "show"))) {
		QJsonObject show;

		// If contains a show object, get show and artist from that
		if (song.contains("show")) {
			show = song["show"].toObject();

			ArtistName = show["publisher"].toString();
		}
		// Otherwise from album if it is used as a show
		else {
			show = song["album"].toObject();

			// Artist name is stored in type for some reason
			// Will return the podcast name instead as it does not store the artist name when stored in an album
			ArtistName = song["artists"].toArray()[0].toObject()["type"].toString();
		}

		// No track number so use 1
		AlbumTrackNumber = 1;
		DiscNumber = 1;

		// Artists
		ArtistNamesList = QStringList{ ArtistName };
		ArtistNames = ArtistName;

		// Album
		AlbumName = show["name"].toString();

		// Check if any images where returned, rare case that they arent
		QJsonArray images = show["images"].toArray();
		if (images.count() != 0)
			AlbumImageURL = images[0].toObject()["url"].toString();

		AlbumArtistNamesList = ArtistNamesList;
		AlbumArtistNames = ArtistNames;

		// Release Date
		QStringList dates = song["release_date"].toString().split("-");

		if (dates.length() == 0) { dates.append("1970"); dates.append("1"); dates.append("1"); } // No Date			   => 1970-1-1
		else if (dates.length() == 1) { dates.append("1"); dates.append("1"); }					 // Format: Year	   => Year-1-1
		else if (dates.length() == 2) { dates.append("1"); }									 // Format: Year-Month => Year-Month-1

		ReleaseDate = QDate(dates[0].toInt(), dates[1].toInt(), dates[2].toInt());
	}
	// Handle regular track
	else {
		// Get Song Details
		if (album.isEmpty()) album = song["album"].toObject();

		// Track Number
		AlbumTrackNumber = song["track_number"].toInt();
		DiscNumber = song["disc_number"].toInt();

		// Artists
		ArtistName = song["artists"].toArray()[0].toObject()["name"].toString();
		ArtistsList = song["artists"].toArray();

		ArtistNamesList = QStringList();
		foreach(QJsonValue artist, ArtistsList) {
			ArtistNamesList.append(artist["name"].toString());
		}
		ArtistNames = ArtistNamesList.join(Config::ArtistSeparator);

		// Album
		AlbumName = album["name"].toString();

		// Check if any images where returned, rare case that they arent
		QJsonArray images = album["images"].toArray();
		if(images.count() != 0)
			AlbumImageURL = images[0].toObject()["url"].toString();
		
		AlbumArtistsList = album["artists"].toArray();

		AlbumArtistNamesList = QStringList();
		foreach(QJsonValue artist, AlbumArtistsList) {
			AlbumArtistNamesList.append(artist["name"].toString());
		}
		AlbumArtistNames = AlbumArtistNamesList.join(Config::ArtistSeparator);

		// Release Date
		QStringList dates = album["release_date"].toString().split("-");

		if (dates.length() == 0) { dates.append("1970"); dates.append("1"); dates.append("1"); } // No Date			   => 1970-1-1
		else if (dates.length() <= 1) { dates.append("1"); dates.append("1"); }					 // Format: Year	   => Year-1-1
		else if (dates.length() <= 2) { dates.append("1"); }									 // Format: Year-Month => Year-Month-1

		ReleaseDate = QDate(dates[0].toInt(), dates[1].toInt(), dates[2].toInt());
	}

	// Set Codec
	Codec = codec;

	// Generate File Name
	if (_main != nullptr)
		GenerateFileName(_main);

	// Generate Downloading Path
	GenerateDownloadingPath();
}

int Song::TrackNumber() {
	switch (Config::TrackNumberIndex) {
		case 0: // Playlist Track Number
			// Return album track number if song not in playlist
			return InPlaylist ? PlaylistTrackNumber : AlbumTrackNumber;
		case 1: // Album Track Number
			return AlbumTrackNumber;
		case 2: // Disk Track Number
			return DiscNumber;
	}

	// Shouldnt reach here but just incase
	return AlbumTrackNumber;
}

std::tuple<QString, bool> Song::TagHandler(Song song, QString tag) {
	QString tagReplacement = QString();
	int indexOfTag = Config::NAMING_TAGS.indexOf(tag.toLower());

	switch (indexOfTag) {
		case 0: // Song Name
			tagReplacement = song.Title;
			break;
		case 1: // Album Name
			tagReplacement = song.AlbumName;
			break;
		case 2: // Song Artist
			tagReplacement = song.ArtistName;
			break;
		case 3: // Song Artists
			for (int i = 0; i < song.ArtistNamesList.count(); i++) {
				QString artistName = song.ArtistNamesList[i];
				tagReplacement.append(artistName);
				if (i < song.ArtistNamesList.count() - 1)
					tagReplacement.append(", ");
			}
			break;
		case 4: // Album Artist
			tagReplacement = song.AlbumArtistNamesList[0];
			break;
		case 5: // Album Artists
			for (int i = 0; i < song.AlbumArtistNamesList.count(); i++) {
				QString artistName = song.AlbumArtistNamesList[i];
				tagReplacement.append(artistName);
				if (i < song.AlbumArtistNamesList.count() - 1)
					tagReplacement.append(", ");
			}
			break;
		case 6: // Codec
			tagReplacement = Codec::Data[song.Codec].String;
			break;
		case 7: // Track Number
			tagReplacement = QString::number(song.TrackNumber());
			break;
		case 8: // Playlist Track Number
			tagReplacement = QString::number(song.PlaylistTrackNumber);
			break;
		case 9: // Album Track Number
			tagReplacement = QString::number(song.AlbumTrackNumber);
			break;
		case 10: // Disk Number
			tagReplacement = QString::number(song.DiscNumber);
			break;
		case 11: // Song Time Seconds
			tagReplacement = QString::number((int)song.Time);
			break;
		case 12: // Song Time Minutes
			tagReplacement = QDateTime::fromSecsSinceEpoch(song.Time, Qt::UTC).toString("mm:ss");
			tagReplacement = tagReplacement.replace(":", "."); // Cannot have colon in file name
			break;
		case 13: // Song Time Hours
			tagReplacement = QDateTime::fromSecsSinceEpoch(song.Time, Qt::UTC).toString("hh:mm:ss");
			tagReplacement = tagReplacement.replace(":", "."); // Cannot have colon in file name
			break;
		case 14: // Year
			tagReplacement = QString::number(song.ReleaseDate.year());
			break;
		case 15: // Month
			tagReplacement = QString::number(song.ReleaseDate.month());
			break;
		case 16: // Day
			tagReplacement = QString::number(song.ReleaseDate.day());
			break;
	}

	// Value was set if index is from 0-tag length
	bool valueSet = indexOfTag >= 0 && indexOfTag <= Config::NAMING_TAGS.length();

	return std::make_tuple(tagReplacement, valueSet);
}

std::tuple<QString, Config::NamingError> Song::OutputNameWithTags(Song song) {
	return Config::FormatStringWithTags(Config::FileNameTag, Config::FileName, true, [=](QString tag) -> std::tuple<QString, bool> { return TagHandler(song, tag); });
}

std::tuple<QString, Config::NamingError> Song::SubFoldersWithTags(Song song) {
	return Config::FormatStringWithTags(Config::SubFoldersTag, Config::SubFolders, true, [=](QString tag) -> std::tuple<QString, bool> { return TagHandler(song, tag); });
}

void Song::GenerateFileName(const SpotifyDownloader* main) {

	// Generate file name, replacing tags for values
	std::tuple<QString, Config::NamingError> filenameData = Song::OutputNameWithTags(*this);

	// No need to check error, was already checked in setup
	FileName = std::get<0>(filenameData);
	FileName = FileUtils::ValidateFileName(FileName);
}

void Song::GenerateDownloadingPath() {
	_tempPath = QString("%1/SpotifyDownloader").arg(QDir::temp().path());
	if (!QDir(_tempPath).exists())
		QDir().mkdir(_tempPath);

	_downloadingFolder = QString("%1/Downloading").arg(_tempPath);
	_downloadingPath = QString("%1/%2.%3").arg(_downloadingFolder).arg(FileName).arg(Codec::Data[Codec].String);

	if (!QDir(_downloadingFolder).exists())
		QDir().mkdir(_downloadingFolder);
}

void Song::DownloadCoverImage() {
	// Return if album image url was not found
	if (AlbumImageURL.isEmpty()) {
		qWarning() << QString("Could not download cover image for (%1).").arg(SpotifyId);
		return;
	}

	QString coverFilename = QString("%1(%2)_Cover").arg(AlbumName).arg(AlbumArtistNames);
	coverFilename = FileUtils::ValidateFileName(coverFilename);
	QString imageTempPath = QString("%1/Cover Art").arg(_tempPath);
	QString imageFileDir = QString("%1/%2.png").arg(imageTempPath).arg(coverFilename);

	if (QFile::exists(imageFileDir)) {
		CoverImage = QImage();
		CoverImage.load(imageFileDir);
	}

	if (!QDir(imageTempPath).exists()) QDir().mkdir(imageTempPath);
	if (!QFile::exists(imageFileDir))
		ImageUtils::DownloadImage(AlbumImageURL, imageFileDir, QSize(640, 640));

	CoverImage = QImage();
	CoverImage.load(imageFileDir);
}

QString Song::SearchForSong(YTMusicAPI*& yt, std::function<void(float)> onProgressUpdate) {
	// Multiple queries as some songs will only get picked up by no quotes, and others with
	QStringList searchQueries{
		QString("%1 - %2 - %3").arg(ArtistName).arg(Title).arg(AlbumName),
		QString(R"(%1 - "%2" - %3)").arg(ArtistName).arg(Title).arg(AlbumName),
		QString("%1 - %2").arg(Title).arg(AlbumName)
	};

	float totalSearches = searchQueries.count() * 3; // For progress bar, float for division

	// Search for songs
	QJsonArray searchResults = QJsonArray();
	for (int i = 0; i < searchQueries.count(); i++) {
		QString searchQuery = searchQueries[i];

		// Search for songs
		searchResults = JSONUtils::Extend(searchResults, yt->Search(searchQuery, "songs", 4));
		onProgressUpdate(MathUtils::Lerp(0, 0.8, (i * 3 + 1) / totalSearches));

		// Search for videos
		searchResults = JSONUtils::Extend(searchResults, yt->Search(searchQuery, "videos", 4));
		onProgressUpdate(MathUtils::Lerp(0, 0.8, (i * 3 + 2) / totalSearches));

		// Search through first album result
		QJsonArray albumSearchResults = yt->Search(searchQuery, "albums", 1);
		foreach(QJsonValue val, albumSearchResults) {
			QJsonObject result = val.toObject();

			QJsonArray albumSongs = yt->GetAlbumTracks(result["browseId"].toString());

			if (albumSongs.isEmpty()) continue;

			searchResults = JSONUtils::Extend(searchResults, albumSongs);
		}
		onProgressUpdate(MathUtils::Lerp(0, 0.8, (i * 3 + 3) / totalSearches));
	}

	// Score all songs
	QJsonArray finalResults = ScoreSearchResults(searchResults);

	onProgressUpdate(0.9);

	// Choose the result with the highest score
	QJsonObject finalResult;
	if (finalResults.count() > 0) {
		// Get result with highest score
		float highestScore = 0;
		for (int i = 0; i < finalResults.count(); i++) {
			QJsonObject result = finalResults[i].toObject();
			if (result["score"].toDouble() > highestScore) {
				highestScore = result["score"].toDouble();
				finalResult = result;
			}
		}

		// If final result is a video, take views into account
		if (finalResult["result"].toObject()["resultType"].toString() == "video") {
			std::vector<double> finalViews = std::vector<double>();
			QJsonArray newFinalResults = QJsonArray();
			foreach(QJsonValue val, finalResults) finalViews.push_back(val.toObject()["views"].toDouble());
			for (int i = 0; i < finalResults.count(); i++) {
				QJsonObject result = finalResults[i].toObject();
				if (result["result"].toObject()["resultType"].toString() == "video" && result["views"].toInt() > 0) {
					double value = MathUtils::LerpInList(finalViews, i);
					newFinalResults.append(QJsonObject{
						{"result", result["result"].toObject()},
						{"score", result["score"].toDouble() + value},
						{"views", result["viewCount"].toInt()}
					});
				}
				else newFinalResults.append(result);
			}

			// Get result with new highest score
			highestScore = 0;
			for (int i = 0; i < finalResults.count(); i++) {
				QJsonObject result = finalResults[i].toObject();
				if (result["score"].toDouble() > highestScore) {
					highestScore = result["score"].toDouble();
					finalResult = result;
				}
			}
		}

	} else {
		return "Song Cannot Be Found On YouTube";
	}

	onProgressUpdate(1);

	finalResult = finalResult["result"].toObject();
	YoutubeId = finalResult["videoId"].toString();
	_searchResult = finalResult;

	return "";
}

QJsonArray Song::ScoreSearchResults(QJsonArray searchResults) {
	QJsonArray finalResults = QJsonArray();

	// Some song IDs can show up multiple times with different durations
	// If this happens, if any of them show a longer than permitted time, add it to this list
	QStringList bannedIDs;

	foreach(QJsonValue val, searchResults) {
		QJsonObject result = val.toObject();

		// If song is explicit only allow explicit results
		if (IsExplicit && !result["isExplicit"].toBool())
			continue;

		// If song in banned IDs, skip
		if (bannedIDs.contains(result["videoId"].toString()))
			continue;

		int seconds = result["durationSeconds"].toInt();

		// If outside the time range, add to banned IDs and skip
		if (seconds == 0 || seconds < Time - 15 || seconds > Time + 15) {
			bannedIDs.append(result["videoId"].toString());
			continue;
		}

		// Score the song

		float totalScore = 0;

		// Title score
		float titleScore = StringUtils::LevenshteinDistanceSimilarity(result["title"].toString(), Title);
		totalScore += titleScore;

		// Time score
		float timeScore = MathUtils::Lerp(0, 1, (15 - abs(seconds - Time)) / 15);
		totalScore += timeScore;

		// Check if time and title are similar enough combined
		if (timeScore < 0.75 && titleScore < 0.5) continue;

		// Artists score
		if (result.contains("artists")) {
			foreach(QJsonValue artist, result["artists"].toArray()) {
				if (ArtistNamesList.contains(artist.toObject()["name"].toString())) {
					totalScore += 1.0;
					break;
				}
			}
		}
		else continue;

		// Album score
		if (result.contains("album")) {
			if (!result["album"].toObject().isEmpty() && result["album"].toObject()["name"].toString().contains(AlbumName)) {
				if (result["album"].toObject()["name"].toString() == AlbumName) totalScore += 0.4;
				else totalScore += 0.3;
			}
		}

		// Title score
		if (result.contains("title")) {
			bool hasTitle = false;
			QString title = result["title"].toString();
			if (title.contains(Title) || Title.contains(title)) {
				if (title == Title) totalScore += 0.5;
				else totalScore += 0.3;
				hasTitle = true;
			}

			// Keywords that are not allowed unless they are in the spotify song title as well
			// Will only work in english titled songs
			QStringList bannedKeywords = {
				"reverse", "instrumental"
			};
			QStringList bannedKeywordsAdditions = {
				" %1 ", "(%1", "%1)", "%1"
			};

			bool banned = false;
			foreach(QString keyword, bannedKeywords) {
				foreach(QString addition, bannedKeywordsAdditions) {
					QString word = addition.arg(keyword);
					if (title.toLower().contains(word) && !Title.toLower().contains(word)) {
						banned = true;
						break;
					}
				}

				if (banned) break;
			}
			if (banned) continue;

			foreach(QString artist, ArtistNamesList) {
				if (title.contains(artist)) {
					totalScore += 0.2;
					break;
				}
			}
		}

		int viewCount = 0;
		if (result["resultType"].toString() == "video" && result.contains("views")) {
			viewCount = StringUtils::StringNumberToInt(result["views"].toString());
		}

		finalResults.append(QJsonObject{
			{"result", result},
			{"score", totalScore},
			{"views", viewCount}
		});
	}

	// Double check banned IDs, songs may have scored with a lower time before it was added
	for (int i = finalResults.count() - 1; i >= 0; i--) {
		QJsonObject result = finalResults[i].toObject();

		if (bannedIDs.contains(result["videoId"].toString()))
			finalResults.removeAt(i);
	}

	return finalResults;
}

QString Song::Download(YTMusicAPI*& yt, QProcess*& process, bool overwrite, std::function<void(float)> onProgressUpdate, std::function<void()> onPOTokenWarning, std::function<void()> onLowQualityWarning, std::function<void()> onPremiumDisabled) {
	// Use downloading path without codec, will be changed after
	QString downloadingPathM4A = QString("%1/%2.m4a").arg(_downloadingFolder).arg(FileName);

	// Remove from temp folder if exists
	if (overwrite && QFile::exists(downloadingPathM4A))
		QFile::remove(downloadingPathM4A);

	// Setup Process
	process = new QProcess();
	QObject::connect(process, &QProcess::finished, process, &QProcess::deleteLater);
	QObject::connect(process, &QProcess::readyRead, process, [&]() {
		QString output = process->readAll();
		if (output.contains("[download]") && !output.contains(FileName)) { // Make sure that it is a download status output and not another file related thing
			QString progress = output.split("]")[1].split("%")[0].replace(" ", "");
			float percent = progress.toFloat() / 100;
			// 0-1 if not converting, 0-0.7 otherwise
			percent = Codec == Codec::Extension::M4A ? MathUtils::Lerp(0, 1, percent) : MathUtils::Lerp(0, 0.7, percent);
			onProgressUpdate(percent);
		}
	});

	// Get youtube cookies file path
	QString cookiesFilePath = QString("%1/cookies.txt").arg(_tempPath);
	bool cookiesAssigned = !Config::YouTubeCookies.isEmpty();

	// Check if cookies file has been deleted during download and if so, re-create it
	QFileInfo cookiesFileInfo(cookiesFilePath);
	if (!cookiesFileInfo.exists() && cookiesAssigned) {
		// Create file
		QFile cookiesFile(cookiesFilePath);
		cookiesFile.open(QIODevice::WriteOnly);

		// Write cookies to file
		QTextStream out(&cookiesFile);
		out << Config::YouTubeCookies;
	}

	// Check if video is age restricted
	// If so and not using cookies, cancel download
	// If using cookies cannot download non-age restricted song with cookies if no premium, dont use cookies in that case)
	bool ageRestricted = yt->IsAgeRestricted(_searchResult["videoId"].toString());
	if (ageRestricted && !cookiesAssigned) {
		return "Video is Age Restricted. Please assign cookies in the downloading settings";
	}

	// Check current hasPremium, might change during download
	bool startingHasPremium = Config::HasPremium;

	// Download song
	// Using --no-part because after killing mid-download, .part files stay in use and cant be deleted
	// I would use --audio-format here but some formats give "Sign in to confirm you are not a bot" errors
	process->startCommand(QString(R"("%1" --no-part -v --extractor-args "youtube:player_client=web_music" %2 -f m4a/bestaudio/best -o "%3" --ffmpeg-location "%4" -x --audio-quality 0 "%5")")
		.arg(QCoreApplication::applicationDirPath() + "/" + _ytdlpPath)
		.arg(QString("--extractor-args \"youtube:po_token=web_music.gvs+%1\" --cookies \"%2\"").arg(Config::POToken).arg(cookiesFilePath))
		.arg(downloadingPathM4A)
		.arg(QCoreApplication::applicationDirPath() + "/" + _ffmpegPath)
		.arg(QString("https://%1.youtube.com/watch?v=%2").arg("music").arg(_searchResult["videoId"].toString()))); // YT Music with cookies, YT without
	process->waitForFinished(-1);

	// Check for any errors in the download
	// I would preferably check some of these when searching to skip the song but no way of checking there
	QString errorOutput = process->readAllStandardError();
	if (!errorOutput.isEmpty()) {
		QString lowerErrorOutput = errorOutput.toLower();

		// If started download assuming premium then other thread assigned premium to false, restart download without cookies
		// This usually happens when starting a download with multiple threads
		if (startingHasPremium && !Config::HasPremium) {
			return Download(yt, process, overwrite, onProgressUpdate, onPOTokenWarning, onLowQualityWarning, onPremiumDisabled);
		}

		// Check if PO Token is invalid, dont cancel download just warn user
		if (lowerErrorOutput.contains("invalid po_token configuration")) {
			qWarning() << SpotifyId << "YT-DLP Returned warning:" << errorOutput;
			qWarning() << "PO Token is invalid";

			// Disable premium as cookies are not being used
			Config::HasPremium = false;

			// Call PO Token warning callback if assigned
			if (onPOTokenWarning != nullptr)
				onPOTokenWarning();
		}

		// Check for error 403, means cookies have expired if set, otherwise forbidden
		if (lowerErrorOutput.contains("http error 403: forbidden")) {
			qWarning() << SpotifyId << "YT-DLP Returned error:" << errorOutput;

			if (cookiesAssigned)
				return "Your cookies have expired. Please reset them and the PO Token";

			// This shouldnt happen but return just incase
			return "HTTP Error 403: Forbidden. Please set cookies";
		}

		// Check if IP was flagged
		if (lowerErrorOutput.contains("sign in to confirm you’re not a bot")) {
			qWarning() << SpotifyId << "YT-DLP Returned error:" << errorOutput;
			return "IP was flagged. Try adding cookies or enabling/disabling a vpn";
		}

		// If video is drm protected, cannot be downloaded
		if (lowerErrorOutput.contains("drm protected") && !lowerErrorOutput.contains("tv client https formats have been skipped as they are drm protected")) {
			qWarning() << SpotifyId << "YT-DLP Returned error:" << errorOutput;
			return "Video is DRM protected";
		}

		// If the video does not have a m4a file (majority should)
		if (lowerErrorOutput.contains("requested format is not available")) {
			qWarning() << SpotifyId << "YT-DLP Returned error:" << errorOutput;
			return "Video does not have file to download";
		}

		// Check for invalid cookies, use the bitrate to check
		if (cookiesAssigned) {
			// Get bitrate by inputting the audio into ffmpeg and reading its output
			process = new QProcess();
			QObject::connect(process, &QProcess::finished, process, &QProcess::deleteLater);
			process->startCommand(QString(R"("%1" -i "%2")")
				.arg(QCoreApplication::applicationDirPath() + "/" + _ffmpegPath)
				.arg(downloadingPathM4A));
			process->waitForFinished(-1);

			QString ffmpegOutput = process->readAllStandardError();
			QStringList outputLines = ffmpegOutput.split("\n");

			foreach(QString outputLine, outputLines) {
				// Look for the first (and only) stream which is the audio
				if (!outputLine.contains("Stream #0:0"))
					continue;

				// Check for after the last "," and before "kb/s"
				// Example line:
				// Stream #0:0[0x1](und): Audio: aac (LC) (mp4a / 0x6134706D), 44100 Hz, stereo, fltp, 128 kb/s (default)

				int kbps = outputLine.split(",").last().split("kb/s")[0].replace(" ", "").toInt();

				// If bitrate is ~128kb/s and premium is enabled, disable it
				if (kbps < 200 && Config::HasPremium) {
					// Check that song has a 256kb/s version
					// If cookies assigned, "Decrypted nsig" will be shown 3 times, if less the video only has a 128kb/s version
					if (lowerErrorOutput.count("decrypted nsig") < 3) {
						qInfo() << SpotifyId << "YT-DLP Response:" << errorOutput;
						qInfo() << SpotifyId << "Cannot be downloaded at 256kb/s with the YouTube ID:" << YoutubeId;

						onLowQualityWarning();

						break;
					}

					qWarning() << SpotifyId << "YT-DLP Returned error:" << errorOutput;
					qInfo() << "User does not have premium or cookies are expired, disabling checks for later downloads";

					Config::HasPremium = false;

					// Call Premium Disabled warning callback if assigned
					if (onPremiumDisabled != nullptr)
						onPremiumDisabled();
				}

				// Exit loop, already found stream
				break;
			}
		}
	}

	// New yt-dlp version sometimes adds extra .m4a to the end, check for that here
	// Bit of a workaround but couldn't find anything about it on the github page
	QString downloadingPathExtraM4A = QString("%1.m4a").arg(downloadingPathM4A);
	if (QFile::exists(downloadingPathExtraM4A))
		QFile::rename(downloadingPathExtraM4A, downloadingPathM4A);

	// Check if song downloaded incase error wasn't previously picked up
	if (!QFile::exists(downloadingPathM4A)) {
		qWarning() << SpotifyId << "Download Failed. YT-DLP Output:" << errorOutput;

		return "Download failed with an unknown error, try downloading again";
	}

	// No need to convert
	if (Codec == Codec::Extension::M4A) {
		onProgressUpdate(1);
		return "";
	}

	onProgressUpdate(0.7);

	// Get duration for progress bar calculations
	process = new QProcess();
	QObject::connect(process, &QProcess::finished, process, &QProcess::deleteLater);
	process->startCommand(QString(R"("%1" -i "%2" -f null -)")
		.arg(QCoreApplication::applicationDirPath() + "/" + _ffmpegPath)
		.arg(_downloadingPath));
	process->waitForFinished(-1);

	QString audioOutput = process->readAllStandardError();
	QString timeString = audioOutput.split("time=").last().split(" bitrate")[0];
	int ms = QDateTime::fromString(QString("0001-01-01T%1").arg(timeString), Qt::ISODateWithMs).time().msecsSinceStartOfDay();
	
	// Convert m4a to desired codec
	process = new QProcess();
	QObject::connect(process, &QProcess::finished, process, &QProcess::deleteLater);
	QObject::connect(process, &QProcess::readyRead, process, [&]() {
		QString msProgressString = QString(process->readAll()).split("\n")[3].split("=")[1];
		int msProgress = int(msProgressString.toInt() / 1000); // Remove last 3 digits
		float progress = float(msProgress) / float(ms);
		progress = MathUtils::Lerp(0.7, 1, progress);
		onProgressUpdate(progress);
	});

	process->startCommand(QString(R"("%1" -i "%2" -progress - -nostats -q:a 0 %3 "%4")")
		.arg(QCoreApplication::applicationDirPath() + "/" + _ffmpegPath)
		.arg(downloadingPathM4A)
		.arg(Codec::Data[Codec].FFMPEGConversionParams)
		.arg(_downloadingPath));
	process->waitForFinished(-1);

	onProgressUpdate(1);

	// Remove temp m4a file
	QFile::remove(downloadingPathM4A);

	return "";
}

// Audio quality will be somewhere around 256kb/s (not exactly) so change it to desired quality ranging from 33 - 256 in this case (33 is minimum, from there 64, 96, 128, ...)
// If quality is not a multiple of 32 ffmpeg will change it to the closest multiple
// Don't set quality if normalizing, has to be set there regardless
void Song::SetBitrate(QProcess*& process, int bitrate, std::function<void(float)> onProgressUpdate) {
	// Some codecs have a preset bitrate, no need to set, eg. WAV files
	if (Codec::Data[Codec].LockedBitrate)
		return;

	// Get duration for progress bar calculations
	process = new QProcess();
	QObject::connect(process, &QProcess::finished, process, &QProcess::deleteLater);
	process->startCommand(QString(R"("%1" -i "%2" -f null -)")
		.arg(QCoreApplication::applicationDirPath() + "/" + _ffmpegPath)
		.arg(_downloadingPath));
	process->waitForFinished(-1);

	QString audioOutput = process->readAllStandardError();
	QString timeString = audioOutput.split("time=").last().split(" bitrate")[0];
	int ms = QDateTime::fromString(QString("0001-01-01T%1").arg(timeString), Qt::ISODateWithMs).time().msecsSinceStartOfDay();

	// Set bitrate
	process = new QProcess();
	QObject::connect(process, &QProcess::finished, process, &QProcess::deleteLater);
	QObject::connect(process, &QProcess::readyRead, process, [&]() {
		QString msProgressString = QString(process->readAll()).split("\n")[3].split("=")[1];
		int msProgress = int(msProgressString.toInt() / 1000); // Remove last 3 digits
		float progress = float(msProgress) / float(ms);

		onProgressUpdate(progress);
	});

	QString newQualityFullPath = QString("%1/%2.%3").arg(_downloadingFolder).arg(FileName + "_A").arg(Codec::Data[Codec].String);
	process->startCommand(QString(R"("%1" -i "%2" -progress - -nostats -b:a %3k "%4")")
		.arg(QCoreApplication::applicationDirPath() + "/" + _ffmpegPath)
		.arg(_downloadingPath)
		.arg(bitrate)
		.arg(newQualityFullPath));
	process->waitForFinished(-1);

	QFile::remove(_downloadingPath);
	QFile::rename(newQualityFullPath, _downloadingPath);
}

void Song::NormaliseAudio(QProcess*& process, float normalisedAudioVolume, int bitrate, bool* quitting, std::function<void(float)> onProgressUpdate) {
	// Get Audio Data
	process = new QProcess();
	QObject::connect(process, &QProcess::finished, process, &QProcess::deleteLater);
	process->startCommand(QString(R"("%1" -i "%2" -af "volumedetect" -vn -sn -dn -f null -)")
		.arg(QCoreApplication::applicationDirPath() + "/" + _ffmpegPath)
		.arg(_downloadingPath));
	process->waitForFinished(-1);

	// For some reason ffmpeg outputs to StandardError. idk why
	QString audioOutput = process->readAllStandardError();
	if (audioOutput.contains("mean_volume:")) {
		QString normalizedFullPath = QString("%1/%2.%3").arg(_downloadingFolder).arg(FileName + "_N").arg(Codec::Data[Codec].String);

		float meanVolume = audioOutput.split("mean_volume:")[1].split("dB")[0].toFloat();

		if (meanVolume != normalisedAudioVolume) {
			// Get time in ms for progress bar
			QString timeString = audioOutput.split("time=").last().split(" bitrate")[0];
			int ms = QDateTime::fromString(QString("0001-01-01T%1").arg(timeString), Qt::ISODateWithMs).time().msecsSinceStartOfDay();

			float volumeApply = (normalisedAudioVolume - meanVolume) + 0.4; // Adding 0.4 here since normalized is always average 0.4-0.5 off of normalized target IDK why (ffmpeg all over the place)

			process = new QProcess();
			QObject::connect(process, &QProcess::finished, process, &QProcess::deleteLater);
			QObject::connect(process, &QProcess::readyRead, process, [&]() {
				QString progressS = process->readAll();

				QString msProgressString = QString(progressS.split("\n")[3].split("=")[1]);
				int msProgress = int(msProgressString.toInt() / 1000); // Remove last 3 digits
				float progress = float(msProgress) / float(ms);

				onProgressUpdate(progress);
			});
			process->startCommand(QString(R"("%1" -i "%2" -progress - -nostats %3 -af "volume=%4dB" "%5")")
				.arg(QCoreApplication::applicationDirPath() + "/" + _ffmpegPath)
				.arg(_downloadingPath)
				.arg((Codec::Data[Codec].LockedBitrate || bitrate <= 0) ? "" : QString("-b:a %1k").arg(bitrate)) // Only set bitrate if not locked and valid bitrate is passed in
				.arg(volumeApply)
				.arg(normalizedFullPath));
			process->waitForFinished(-1);

			QFile::remove(_downloadingPath);
			QFile::rename(normalizedFullPath, _downloadingPath);

			return;
		}
	}

	// Set bitrate if song doesn't need to be normalised
	SetBitrate(process, bitrate, onProgressUpdate);
}

void Song::AssignMetadata() {
	// Only assign metadata if required
	if (Codec::Data[Codec].Type == Codec::MetadataType::NONE)
		return;

	// FileRef destroys when leaving scope, give it a scope to do its thing
	{
		TagLib::FileRef tagFileRef(FileUtils::ToNativeFilePathTagLib(_downloadingPath), true, TagLib::AudioProperties::Accurate);

		// Only load cover art if image found
		QByteArray imageBytes;
		if (!CoverImage.isNull()) {
			QBuffer buffer(&imageBytes);
			buffer.open(QIODevice::WriteOnly);
			CoverImage.save(&buffer, "PNG");
			buffer.close();
		}

		// Check if the cover art is handled differently
		bool coverArtOverride = Codec::Data[Codec].CoverArtOverride != NULL;

		// Set variables
		TagLib::String title(Title.toUtf8().constData(), TagLib::String::UTF8);
		TagLib::String artists(ArtistNames.toUtf8().constData(), TagLib::String::UTF8);
		TagLib::String albumArtists(AlbumArtistNames.toUtf8().constData(), TagLib::String::UTF8);
		TagLib::String album(AlbumName.toUtf8().constData(), TagLib::String::UTF8);

		TagLib::String publisherText = "Downloaded through Spotify Downloader by William S";
		TagLib::String copyrightText = QString("Spotify ID (%1), Youtube ID (%2)").arg(SpotifyId).arg(YoutubeId).toUtf8().data();
		TagLib::String commentText = "Thanks for using my program! :) - William S";

		TagLib::String compressedComment = QString("%1\n%2\n%3").arg(copyrightText.toWString()).arg(publisherText.toWString()).arg(commentText.toWString()).toUtf8().data();

		TagLib::String releaseDate = QString("%1-%2-%3").arg(ReleaseDate.year()).arg(ReleaseDate.month()).arg(ReleaseDate.day()).toUtf8().data();
		TagLib::String trackNumber = QString::number(TrackNumber()).toUtf8().data();
		TagLib::String discNumber = QString::number(DiscNumber).toUtf8().data();

		// Handle each metadata type
		switch (Codec::Data[Codec].Type) {
			case Codec::MetadataType::ID3V2:
			{
				TagLib::ID3v2::Tag* tag = dynamic_cast<TagLib::ID3v2::Tag*>(Codec::Data[Codec].GetFileTag(tagFileRef));

				// Use preset functions for basic values
				tag->setTitle(title);
				tag->setArtist(artists);
				tag->setAlbum(album);

				// Below work on specific systems where the above dont (#33) need to look further into it
				// tag->setTitle(Title.trimmed().toUtf8().data());
				// tag->setArtist(ArtistNames.trimmed().toUtf8().data());
				// tag->setAlbum(AlbumName.trimmed().toUtf8().data());

				// Create extra frames
				TagLib::ID3v2::TextIdentificationFrame* albumArtistFrame = new TagLib::ID3v2::TextIdentificationFrame("TPE2");
				TagLib::ID3v2::TextIdentificationFrame* trackNumberFrame = new TagLib::ID3v2::TextIdentificationFrame("TRCK");
				TagLib::ID3v2::TextIdentificationFrame* discNumberFrame = new TagLib::ID3v2::TextIdentificationFrame("TPOS");
				TagLib::ID3v2::TextIdentificationFrame* publisherFrame = new TagLib::ID3v2::TextIdentificationFrame("TPUB");
				TagLib::ID3v2::TextIdentificationFrame* copyrightFrame = new TagLib::ID3v2::TextIdentificationFrame("TCOP");
				TagLib::ID3v2::TextIdentificationFrame* releaseDateFrame = new TagLib::ID3v2::TextIdentificationFrame("TDRL");
				TagLib::ID3v2::TextIdentificationFrame* dateFrame = new TagLib::ID3v2::TextIdentificationFrame("TDRC");
				TagLib::ID3v2::CommentsFrame* commentFrame = new TagLib::ID3v2::CommentsFrame();

				// Set frame values
				albumArtistFrame->setText(albumArtists);
				trackNumberFrame->setText(trackNumber);
				discNumberFrame->setText(discNumber);
				publisherFrame->setText(publisherText);
				copyrightFrame->setText(copyrightText);
				commentFrame->setText(commentText);
				releaseDateFrame->setText(releaseDate);
				dateFrame->setText(releaseDate);

				// Add frames to the tag
				tag->addFrame(albumArtistFrame);
				tag->addFrame(trackNumberFrame);
				tag->addFrame(discNumberFrame);
				tag->addFrame(publisherFrame);
				tag->addFrame(copyrightFrame);
				tag->addFrame(commentFrame);
				tag->addFrame(releaseDateFrame);
				tag->addFrame(dateFrame);
				
				// Only set cover art if not being overriden
				if (coverArtOverride || CoverImage.isNull())
					break;

				// Set cover art
				TagLib::ID3v2::AttachedPictureFrame* picFrame = new TagLib::ID3v2::AttachedPictureFrame();
				picFrame->setPicture(TagLib::ByteVector(imageBytes.data(), imageBytes.count()));
				picFrame->setMimeType("image/png");
				picFrame->setType(TagLib::ID3v2::AttachedPictureFrame::FrontCover);
				tag->addFrame(picFrame);

				break;
			}
			case Codec::MetadataType::MP4:
			{
				TagLib::MP4::Tag* tag = dynamic_cast<TagLib::MP4::Tag*>(Codec::Data[Codec].GetFileTag(tagFileRef));

				// Use preset functions for basic values 
				tag->setTitle(title);
				tag->setArtist(artists);
				tag->setAlbum(album);
				tag->setComment(compressedComment);
				tag->setTrack(TrackNumber());

				// Extra values
				tag->setItem("aART", TagLib::StringList(albumArtists)); // Album Artists
				tag->setItem("\251day", TagLib::StringList(releaseDate)); // Date
				tag->setItem("disc", TagLib::StringList(discNumber)); // Disc Number

				// Only set cover art if not being overriden
				if (coverArtOverride || CoverImage.isNull())
					break;

				// Set cover art
				TagLib::MP4::CoverArt coverArt(TagLib::MP4::CoverArt::Format::PNG, TagLib::ByteVector(imageBytes.data(), imageBytes.count()));
				TagLib::MP4::CoverArtList coverArtList;
				coverArtList.append(coverArt);
				TagLib::MP4::Item coverItem(coverArtList);

				tag->setItem("covr", coverItem);

				break;
			}
			case Codec::MetadataType::RIFF:
			{
				TagLib::RIFF::Info::Tag* tag = dynamic_cast<TagLib::RIFF::Info::Tag*>(Codec::Data[Codec].GetFileTag(tagFileRef));

				// Use preset functions for basic values 
				tag->setTitle(title);
				tag->setArtist(artists);
				tag->setAlbum(album);
				tag->setTrack(TrackNumber());
				tag->setComment(compressedComment);

				// Extra values
				tag->setFieldText("ICRD", releaseDate); // Date

				// RIFF doesnt support:
				// - Album Artist
				// - Cover Art

				break;
			}
			case Codec::MetadataType::XIPH:
			{
				TagLib::Ogg::XiphComment* tag = dynamic_cast<TagLib::Ogg::XiphComment*>(Codec::Data[Codec].GetFileTag(tagFileRef));

				// Use preset functions for basic values 
				tag->setTitle(title);
				tag->setArtist(artists);
				tag->setAlbum(album);
				tag->setTrack(TrackNumber());
				tag->setComment(compressedComment);

				// Extra values
				tag->addField("ALBUMARTIST", albumArtists);
				tag->addField("RELEASEDATE", releaseDate);
				tag->addField("DATE", releaseDate);
				tag->addField("DISCNUMBER", discNumber);

				// Only set cover art if not being overriden
				if (coverArtOverride || CoverImage.isNull())
					break;

				// Set cover art
				TagLib::FLAC::Picture* coverArt = new TagLib::FLAC::Picture();
				coverArt->setData(TagLib::ByteVector(imageBytes.data(), imageBytes.count()));
				coverArt->setMimeType("image/png");
				coverArt->setType(TagLib::FLAC::Picture::Type::FrontCover);
				tag->addPicture(coverArt);

				break;
			}
		}

		// Call cover art function if set
		if (coverArtOverride)
			Codec::Data[Codec].CoverArtOverride(tagFileRef, imageBytes);

		// Save metadata
		tagFileRef.save();
	}
}

void Song::Save(QString targetFolder, QString targetPath, bool overwrite) {
	// Remove from target folder if file exists
	if (overwrite && QFile::exists(targetPath))
		QFile::remove(targetPath);

	// Check each folder in target folder and create them if they doesnt exist
	// Start with second folder (Drive/Folder), dont check for drive
	QStringList folders = targetFolder.split("/");
	QString currentFolder = QString("%1/%2").arg(folders[0]).arg(folders[1]);
	for (int i = 1; i < folders.count(); i++) {
		if (!QDir(currentFolder).exists())
			QDir().mkdir(currentFolder);

		if (i + 1 < folders.count())
			currentFolder = QString("%1/%2").arg(currentFolder).arg(folders[i + 1]);
	}

	// Move from downloading path to target path
	QFile::rename(_downloadingPath, targetPath);
}