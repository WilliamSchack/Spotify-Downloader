#include "SpotifyDownloader.h"

#pragma region Utilities On Main Thread
void SpotifyDownloader::ChangeScreen(int screenIndex) {
	_ui.Screens->setCurrentIndex(screenIndex);
}
void SpotifyDownloader::SetProgressLabel(QString text) {
	_ui.ProgressLabel->setText(text);
}
void SpotifyDownloader::SetProgressBar(float percentage) {
	_ui.ProgressBar_Front->resize(SongDownloader::Lerp(0, 334, percentage), 27);
}
void SpotifyDownloader::SetSongCount(int currentCount, int totalCount) {
	_ui.SongCount->setText(QString("%1/%2").arg(QString::number(currentCount)).arg(QString::number(totalCount)));
	_ui.SongCount->adjustSize();
}
void SpotifyDownloader::SetSongImage(QPixmap image) {
	image = image.scaled(_ui.SongImage->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	_ui.SongImage->setPixmap(image);
}
void SpotifyDownloader::SetSongDetails(QString title, QString artists) {
	_ui.SongTitle->setText(title);
	_ui.SongArtists->setText(artists);
}
void SpotifyDownloader::SetErrorItems(QJsonArray tracks) {
	foreach(QJsonValue val, tracks) {
		QJsonObject track = val.toObject();

		SongErrorItem *errorItem = new SongErrorItem();
		errorItem->Title->setText(track["title"].toString());
		errorItem->Album->setText(track["album"].toString());
		errorItem->Artists->setText(track["artists"].toString());
		errorItem->Image->setPixmap(JSONUtils::PixmapFromJSON(track["image"]).scaled(errorItem->Image->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

		_ui.ScrollLayout->insertWidget(_ui.ScrollLayout->count() - 1, errorItem);
	}
}
void SpotifyDownloader::HidePauseWarning() {
	_ui.PauseWarning->hide();
	_ui.ProgressLabel->setText("Paused...");
}
#pragma endregion

void SongDownloader::DownloadSongs(const SpotifyDownloader* main) {
	Main = main;

	emit SetProgressLabel("Getting Playlist Data...");

	QString url = Main->PlaylistURLText;
	QString spotifyId = url.split("/").last().split("?")[0];
	QJsonArray tracks = QJsonArray();
	QJsonObject album = QJsonObject();

	_yt = YTMusicAPI();

	SpotifyAPI sp = SpotifyAPI();
	if (url.contains("playlist")) tracks = sp.GetPlaylistTracks(spotifyId);
	else if (url.contains("album")) {
		album = sp.GetAlbum(spotifyId);
		tracks = sp.GetAlbumTracks(album);
	}
	else tracks = QJsonArray{ sp.GetTrack(spotifyId) };

	_totalSongCount = tracks.count();
	emit SetSongCount(0, _totalSongCount);

	_tracksNotFound = QJsonArray();
	for (int i = 0; i < _totalSongCount; i++) {
		QJsonObject track = tracks[i].toObject();
		if (url.contains("playlist")) track = track["track"].toObject();

		DownloadSong(track, i, album);
	}

	if (_tracksNotFound.count() == 0) {
		emit ChangeScreen(2);
		return;
	}

	emit ChangeScreen(3);
	emit SetErrorItems(_tracksNotFound);
}

void SongDownloader::DownloadSong(QJsonObject track, int count, QJsonObject album) {
	#pragma region Get Song Details
	if (album.isEmpty()) album = track["album"].toObject();

	QString trackTitle = track["name"].toString();
	QString albumName = album["name"].toString();
	QString artistName = track["artists"][0]["name"].toString();
	QJsonArray songArtistsList = track["artists"].toArray();
	QJsonArray albumArtistsList = album["artists"].toArray();
	float songTime = track["duration_ms"].toDouble() / 1000;
	QString songId = track["id"].toString();
	QString albumImageURL = album["images"].toArray()[0].toObject()["url"].toString();

	QStringList songArtistNamesList = QStringList();
	foreach(QJsonValue artist, songArtistsList) {
		songArtistNamesList.append(artist["name"].toString());
	}
	QString songArtistNames = songArtistNamesList.join("; ");

	QStringList albumArtistNamesList = QStringList();
	foreach(QJsonValue artist, albumArtistsList) {
		albumArtistNamesList.append(artist["name"].toString());
	}
	QString albumArtistNames = albumArtistNamesList.join("; ");

	QString filename = QString("%1 - %2").arg(trackTitle).arg(artistName);
	filename = ValidateString(filename);
	QString path = QString("%1/%2").arg(Main->SaveLocationText).arg(filename);
	QString fullPath = QString("%1.%2").arg(path).arg(CODEC);

	if (!Main->Overwrite && QFile::exists(fullPath)) return;
	#pragma endregion

	CheckIfPaused();

	#pragma region Download Cover Art
	emit SetProgressLabel("Downloading Cover Art...");
	QString coverFilename = QString("%1(%2)_Cover").arg(albumName).arg(albumArtistNames);
	coverFilename = ValidateString(coverFilename);
	QString tempPath = QString("%1/SpotifyDownloader").arg(QDir::temp().path());
	QString imageFileDir = QString("%1/%2.png").arg(tempPath).arg(coverFilename);
	if (!QDir(tempPath).exists()) QDir().mkdir(tempPath);
	if (!QFile::exists("imageFileDir")) {
		DownloadImage(albumImageURL, imageFileDir, QSize(640, 640));
	}
	QImage image;
	image.load(imageFileDir);
	#pragma endregion

	CheckIfPaused();

	#pragma region Assign Data To GUI
	emit SetSongImage(QPixmap::fromImage(image));
	emit SetSongDetails(trackTitle, songArtistNames.replace(";", ","));
	emit SetSongCount(count + 1, _totalSongCount);
	#pragma endregion

	CheckIfPaused();

	#pragma region Search For Song
	emit SetProgressLabel("Searching...");
	QString searchQuery = QString(R"(%1 - "%2" - %3)").arg(artistName).arg(trackTitle).arg(albumName);
	QJsonArray searchResults = _yt.Search(searchQuery, "songs", 6);
	searchResults = JSONUtils::Extend(searchResults, _yt.Search(searchQuery, "videos", 6));
	
	QJsonArray finalResults = QJsonArray();
	foreach(QJsonValue val, searchResults) {
		QJsonObject result = val.toObject();
		
		int seconds = result["durationSeconds"].toInt();

		if (seconds != 0 && seconds > songTime - 15 && seconds < songTime + 15) {
			// Title score
			float totalScore = difflib::MakeSequenceMatcher(result["title"].toString().toStdString(), trackTitle.toStdString()).ratio();
			// Time score
			float timeScore = Lerp(0, 1, (15 - abs(seconds - songTime)) / 15);
			totalScore += timeScore;

			// Artists score
			if (result.contains("artists")) {
				foreach(QJsonValue artist, result["artists"].toArray()) {
					if (songArtistNamesList.contains(artist.toString())) {
						totalScore += 0.4;
						break;
					}
				}
			} else continue;

			// Album score
			if (result.contains("album")) {
				if (!result["album"].toObject().isEmpty() && result["album"].toObject()["name"].toString().contains(albumName)) {
					if (result["album"].toObject()["name"].toString() == albumName) totalScore += 0.4;
					else totalScore += 0.3;
				}
			}

			// Title score
			if (result.contains("title")) {
				bool hasTitle = false;
				QString title = result["title"].toString();
				if (title.contains(trackTitle) || trackTitle.contains(title)) {
					if (title == trackTitle) totalScore += 0.5;
					else totalScore += 0.3;
					hasTitle = true;
				}

				QStringList bannedKeywords = {
					" reverse ", "(reverse", "reverse)", "(reverse)",
					" reversed ", "(reversed", "reversed)", "(reversed)",
					" instrumental ", "(instrumental", "instrumental)", "(instrumental)"
				};
				bool banned = false;
				foreach(QString keyword, bannedKeywords) {
					if (title.toLower().contains(keyword)) {
						banned = true;
						break;
					}
				}
				if (banned) continue;

				if (timeScore < 0.8 && !hasTitle) continue;

				foreach(QString artist, songArtistNamesList) {
					if (title.contains(artist)) {
						totalScore += 0.2;
						break;
					}
				}
			}

			int viewCount = 0;
			if (result["resultType"].toString() == "video" && result.contains("views")) viewCount = result["views"].toInt();

			finalResults.append(QJsonObject{
				{"result", result},
				{"score", totalScore},
				{"views", viewCount}
			});
		}
	}

	// Choose the result with the highest score
	QJsonObject finalResult;
	if (finalResults.count() > 0) {
		float highestScore = 0;
		for (int i = 0; i < finalResults.count(); i++) {
			QJsonObject result = finalResults[i].toObject();
			if (result["score"].toDouble() > highestScore) {
				highestScore = result["score"].toDouble();
				finalResult = result;
			}
		}

		if (finalResult["result"].toObject()["resultType"].toString() == "video") {
			std::vector<double> finalViews = std::vector<double>();
			QJsonArray newFinalResults = QJsonArray();
			foreach(QJsonValue val, finalResults) finalViews.push_back(val.toObject()["views"].toDouble());
			for (int i = 0; i < finalResults.count(); i++) {
				QJsonObject result = finalResults[i].toObject();
				if (result["result"].toObject()["resultType"].toString() == "video" && result["views"].toInt() > 0) {
					double value = LerpInList(finalViews, i);
					newFinalResults.append(QJsonObject{
						{"result", result["result"].toObject()},
						{"score", result["score"].toDouble() + value},
						{"views", result["viewCount"].toInt()}
					});
				} else newFinalResults.append(result);
			}
			
			float newHighestScore = 0;
			for (int i = 0; i < finalResults.count(); i++) {
				QJsonObject result = finalResults[i].toObject();
				if (result["score"].toDouble() > newHighestScore) {
					newHighestScore = result["score"].toDouble();
					finalResult = result;
				}
			}
		}

	} else {
		emit SetProgressLabel("SONG NOT FOUND");
		_tracksNotFound.append(QJsonObject{
			{"title", trackTitle},
			{"album", albumName},
			{"artists", songArtistNames},
			{"image", JSONUtils::PixmapToJSON(QPixmap::fromImage(image))}
		});

		QThread::sleep(2);
		return;
	}

	finalResult = finalResult["result"].toObject();
	#pragma endregion

	CheckIfPaused();

	#pragma region Download Track
	emit SetProgressLabel("Downloading Track...");
	emit SetProgressBar(0);

	if (Main->Overwrite && QFile::exists(fullPath)) QFile::remove(fullPath);

	QProcess* downloadProcess = new QProcess();
	connect(downloadProcess, &QProcess::finished, downloadProcess, &QProcess::deleteLater);
	connect(downloadProcess, &QProcess::readyRead, downloadProcess, [&]() {
		QString output = downloadProcess->readAll();
		if (output.contains("%")) {
			QString progress = output.split("]")[1].split("%")[0];
			emit SetProgressBar(progress.toFloat() / 100);
		}
	});
	downloadProcess->startCommand(QString(R"("%1" -f m4a/bestaudio/best -o "%2" --ffmpeg-location "%3" -x --audio-format %4 "%5")")
						.arg(QDir::currentPath() + "/" + YTDLP_PATH)
						.arg(fullPath)
						.arg(QDir::currentPath() + "/" + FFMPEG_PATH)
						.arg(CODEC)
						.arg(QString("https://www.youtube.com/watch?v=%1").arg(finalResult["videoId"].toString())));
	downloadProcess->waitForFinished();

	emit SetProgressBar(1);
	#pragma endregion

	CheckIfPaused();

	#pragma region Normalize Audio
	if (Main->NormalizeAudio) {
		emit SetProgressLabel("Normalizing Audio...");

		QProcess* audioProcess = new QProcess();
		connect(audioProcess, &QProcess::finished, audioProcess, &QProcess::deleteLater);
		audioProcess->startCommand(QString(R"("%1" -i "%2" -af "volumedetect" -vn -sn -dn -f null -)")
						.arg(QDir::currentPath() + "/" + FFMPEG_PATH)
						.arg(fullPath));
		audioProcess->waitForFinished();

		// For some reason ffmpeg outputs to StandardError. idk why
		QString audioOutput = audioProcess->readAllStandardError();
		if (audioOutput.contains("mean_volume:")) {
			QString normalizedFullPath = QString("%1/%2.%3").arg(Main->SaveLocationText).arg(filename + "_N").arg(CODEC);
			
			float meanVolume = audioOutput.split("mean_volume:")[1].split("dB")[0].toFloat();

			if (meanVolume != Main->NormalizeAudioVolume) {
				float volumeApply = (Main->NormalizeAudioVolume - meanVolume) + 0.4; // Adding 0.4 here since normalized is always average 0.4-0.5 off of normalized target IDK why (ffmpeg all over the place)

				QProcess* normalizeProcess = new QProcess();
				connect(normalizeProcess, &QProcess::finished, normalizeProcess, &QProcess::deleteLater);
				normalizeProcess->startCommand(QString(R"("%1" -i "%2" -af "volume=%3" "%4")")
					.arg(QDir::currentPath() + "/" + FFMPEG_PATH)
					.arg(fullPath)
					.arg(QString("%1dB").arg(volumeApply))
					.arg(normalizedFullPath));
				normalizeProcess->waitForFinished();

				QFile::remove(fullPath);
				QFile::rename(normalizedFullPath, fullPath);
			}
		}
	}
	#pragma endregion

	CheckIfPaused();

	#pragma region Assign Metadata
	emit SetProgressLabel("Assigning Metadata...");

	TagLib::MPEG::File file(reinterpret_cast<const wchar_t*>(fullPath.constData()));
	TagLib::ID3v2::Tag* tag = file.ID3v2Tag(true);
	tag->setTitle(reinterpret_cast<const wchar_t*>(trackTitle.constData()));
	tag->setArtist(reinterpret_cast<const wchar_t*>(songArtistNames.constData()));
	tag->setAlbum(reinterpret_cast<const wchar_t*>(albumName.constData()));

	TagLib::ID3v2::TextIdentificationFrame *pubFrame = new TagLib::ID3v2::TextIdentificationFrame("TPUB");
	TagLib::ID3v2::TextIdentificationFrame *copFrame = new TagLib::ID3v2::TextIdentificationFrame("TCOP");
	TagLib::ID3v2::CommentsFrame *comFrame = new TagLib::ID3v2::CommentsFrame();
	pubFrame->setText("William S - Spotify Downloader");
	copFrame->setText(QString("Spotify ID (%1), Youtube ID (%2)").arg(songId).arg(finalResult["videoId"].toString()).toStdString().data());
	comFrame->setText("Thanks for using my program! :)\n- William S");
	tag->addFrame(pubFrame);
	tag->addFrame(copFrame);
	tag->addFrame(comFrame);

	QByteArray imageBytes;
	QBuffer buffer(&imageBytes);
	buffer.open(QIODevice::WriteOnly);
	image.save(&buffer, "PNG");

	TagLib::ID3v2::AttachedPictureFrame* picFrame = new TagLib::ID3v2::AttachedPictureFrame();
	picFrame->setPicture(TagLib::ByteVector(imageBytes.data(), imageBytes.count()));
	picFrame->setMimeType("image/png");
	picFrame->setType(TagLib::ID3v2::AttachedPictureFrame::FrontCover);
	tag->addFrame(picFrame);

	file.save();
	#pragma endregion
}

void SongDownloader::CheckIfPaused() {
	if (!Main->Paused) return;

	emit HidePauseWarning();
	while (Main->Paused) {
		QCoreApplication::processEvents();
	}
}

QString SongDownloader::ValidateString(QString string) {
	QString invalidChars = R"(<>:"/\|?*)";
	foreach(QChar c, invalidChars) {
		string.remove(c);
	}
	return string;
}

void SongDownloader::DownloadImage(QString url, QString path, QSize resize) {
	QNetworkRequest req(url);
	QByteArray response = Network::Get(req);
	QImage image;
	image.loadFromData(response);
	if(!resize.isEmpty()) image = image.scaled(resize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	image.save(path);
}

float SongDownloader::Lerp(float a, float b, float t) {
	return a * (1.0 - t) + (b * t);
}

double SongDownloader::LerpInList(std::vector<double> list, int index) {
	double maxVal = 0;
	foreach(double val, list) {
		if (val > maxVal) maxVal = val;
	}
	
	return list[0] / maxVal;
}