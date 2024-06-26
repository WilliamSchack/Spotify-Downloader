#include "SpotifyDownloader.h"

#include "Network.h"

#include <difflib.h>

#include <taglib/fileref.h>
#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/id3v2frame.h>
#include <taglib/commentsframe.h>
#include <taglib/attachedpictureframe.h>
#include <taglib/textidentificationframe.h>

void SongDownloader::DownloadSongs(const SpotifyDownloader* main, const PlaylistDownloader* manager, YTMusicAPI* yt, QJsonArray tracks, QJsonObject album, int threadIndex) {
	Main = main;
	Manager = manager;
	_yt = yt;
	_downloadingTracks = tracks;
	_album = album;
	_threadIndex = threadIndex;
	_totalSongCount = _downloadingTracks.count();

	QThread* thread = QThread::currentThread();

	emit SetProgressLabel(_threadIndex, "Getting Playlist Data...");

	StartDownload(0);
}

void SongDownloader::StartDownload(int startIndex) {
	int startingTotalSongCount = _totalSongCount;

	_tracksNotFound = QJsonArray();
	for (int i = startIndex; i < _totalSongCount; i++) {
		QJsonObject track = _downloadingTracks[i].toObject();

		_currentTrack = track;
		DownloadSong(track, i, _album);

		if (_quitting) {
			this->thread()->quit();
			return;
		}

		SongsDownloaded++;
		emit SongDownloaded();


		while (Manager->PauseNewDownloads) {
			QCoreApplication::processEvents();
		}
	}

	_waitingForFinishedResponse = true;
	emit Finish(_threadIndex, _tracksNotFound);
	while (_waitingForFinishedResponse) {
		QCoreApplication::processEvents();
	}

	// If no songs are added and done downloading on thread finish up
	if (_finishedDownloading) return;

	// If songs are added restart the downloading from the new index
	StartDownload(startingTotalSongCount);
}

void SongDownloader::DownloadSong(QJsonObject track, int count, QJsonObject album) {
	#pragma region Get Song Details
	if (album.isEmpty()) album = track["album"].toObject();

	QString trackTitle = track["name"].toString();
	QString albumName = album["name"].toString();
	QString artistName = track["artists"].toArray()[0].toObject()["name"].toString();
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

	// Generate file name, replacing tags for values
	std::tuple<QString, SpotifyDownloader::NamingError> filenameData = Main->FormatOutputNameWithTags([=](QString tag) -> QString {
		QStringList namingTags = Main->Q_NAMING_TAGS();

		QString tagReplacement = QString();
		int indexOfTag = namingTags.indexOf(tag.toLower());
		switch (indexOfTag) {
			case 0: // Song Name
				tagReplacement = trackTitle;
				break;
			case 1: // Album Name
				tagReplacement = albumName;
				break;
			case 2: // Song Artist
				tagReplacement = artistName;
				break;
			case 3: // Song Artists
				for (int i = 0; i < songArtistNamesList.count(); i++) {
					QString artistName = songArtistNamesList[i];
					tagReplacement.append(artistName);
					if (i < songArtistNamesList.count() - 1)
						tagReplacement.append(", ");
				}
				break;
			case 4: // Album Artist
				tagReplacement = albumArtistNamesList[0];
				break;
			case 5: // Album Artists
				for (int i = 0; i < albumArtistNamesList.count(); i++) {
					QString artistName = albumArtistNamesList[i];
					tagReplacement.append(artistName);
					if (i < albumArtistNamesList.count() - 1)
						tagReplacement.append(", ");
				}
				break;
			case 6: // Song Time Seconds
				tagReplacement = QString::number((int)songTime);
				break;
			case 7: // Song Time Minutes
				tagReplacement = QDateTime::fromSecsSinceEpoch(songTime, Qt::UTC).toString("mm:ss");
				tagReplacement = tagReplacement.replace(":", "."); // Cannot have colon in file name
				break;
			case 8: // Song Time Hours
				tagReplacement = QDateTime::fromSecsSinceEpoch(songTime, Qt::UTC).toString("hh:mm:ss");
				tagReplacement = tagReplacement.replace(":", "."); // Cannot have colon in file name
				break;
		}

		return tagReplacement;
	});

	// No need to check error, was already checked in setup
	QString fileName = std::get<0>(filenameData);
	fileName = ValidateString(fileName);

	// Set downloading path
	QString tempPath = QString("%1/SpotifyDownloader").arg(QDir::temp().path());
	if (!QDir(tempPath).exists())
		QDir().mkdir(tempPath);

	QString downloadingFolder = QString("%1/Downloading").arg(tempPath);
	QString downloadingPath = QString("%1/%2.%3").arg(downloadingFolder).arg(fileName).arg(CODEC);
	//QString fullDownloadingPath = QString("%1.%2").arg(downloadingPath).arg(CODEC);

	// Set downloading folder
	QString downloadingFolderName = "";
	switch (Main->FolderSortingIndex) {
		case 1: // Album name
			downloadingFolderName = albumName;
			break;
		case 2: // Song Artist
			downloadingFolderName = artistName;
			break;
		case 3: // Song Artists
			for (int i = 0; i < songArtistNamesList.count(); i++) {
				QString artistName = songArtistNamesList[i];
				downloadingFolderName.append(artistName);
				if (i < songArtistNamesList.count() - 1)
					downloadingFolderName.append(", ");
			}
			break;
		case 4: // Album Artist
			downloadingFolderName = albumArtistNamesList[0];
			break;
		case 5: // Album Artists
			for (int i = 0; i < albumArtistNamesList.count(); i++) {
				QString artistName = albumArtistNamesList[i];
				downloadingFolderName.append(artistName);
				if (i < albumArtistNamesList.count() - 1)
					downloadingFolderName.append(", ");
			}
			break;
	}
	downloadingFolderName = ValidateString(downloadingFolderName);

	// Set target path
	QString targetFolder = QString("%1/%2").arg(Main->SaveLocationText).arg(downloadingFolderName);
	QString targetPath = QString("%1/%2.%3").arg(targetFolder).arg(fileName).arg(CODEC);

	if (!QDir(downloadingFolder).exists())
		QDir().mkdir(downloadingFolder);

	if (!Main->Overwrite && QFile::exists(targetPath)) return;
	#pragma endregion

	if (_quitting) return;
	CheckForStop();

	#pragma region Download Cover Art
	emit SetProgressLabel(_threadIndex, "Downloading Cover Art...");

	QString coverFilename = QString("%1(%2)_Cover").arg(albumName).arg(albumArtistNames);
	coverFilename = ValidateString(coverFilename);
	QString imageTempPath = QString("%1/Cover Art").arg(tempPath);
	QString imageFileDir = QString("%1/%2.png").arg(imageTempPath).arg(coverFilename);

	if (!QDir(imageTempPath).exists()) QDir().mkdir(imageTempPath);
	if (!QFile::exists(imageFileDir))
		DownloadImage(albumImageURL, imageFileDir, QSize(640, 640));

	QImage image;
	image.load(imageFileDir);
	#pragma endregion

	if (_quitting) return;
	CheckForStop();

	#pragma region Assign Data To GUI
	emit SetSongImage(_threadIndex, QPixmap::fromImage(image));
	emit SetSongDetails(_threadIndex, trackTitle, songArtistNames.replace(";", ","));
	emit SetSongCount(_threadIndex, count + 1, _totalSongCount);
	#pragma endregion

	if (_quitting) return;
	CheckForStop();

	#pragma region Search For Song
	emit SetProgressLabel(_threadIndex, "Searching...");

	QString searchQuery = QString(R"(%1 - "%2" - %3)").arg(artistName).arg(trackTitle).arg(albumName);
	QJsonArray searchResults = _yt->Search(searchQuery, "songs", 6);
	searchResults = JSONUtils::Extend(searchResults, _yt->Search(searchQuery, "videos", 6));
	
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
					if (songArtistNamesList.contains(artist.toObject()["name"].toString())) {
						totalScore += 1.0;
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
					"reverse", "reversed", "instrumental"
				};
				QStringList bannedKeywordsAdditions = {
					" %1 ", "(%1", "%1)", "%1"
				};

				bool banned = false;
				foreach(QString keyword, bannedKeywords) {
					foreach(QString addition, bannedKeywordsAdditions) {
						QString word = addition.arg(keyword);
						if (title.toLower().contains(word)) {
							banned = true;
							break;
						}
					}
					if (banned) break;
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
		emit SetProgressLabel(_threadIndex, "SONG NOT FOUND");
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

	if (_quitting) return;
	CheckForStop();

	#pragma region Download Track
	emit SetProgressLabel(_threadIndex, "Downloading Track...");
	emit SetProgressBar(_threadIndex, 0);

	// Remove from temp folder if exists
	if (Main->Overwrite && QFile::exists(downloadingPath))
		QFile::remove(downloadingPath);

	_currentProcess = new QProcess();
	connect(_currentProcess, &QProcess::finished, _currentProcess, &QProcess::deleteLater);
	connect(_currentProcess, &QProcess::readyRead, _currentProcess, [&]() {
		QString output = _currentProcess->readAll();
		if (output.contains("%")) {
			QString progress = output.split("]")[1].split("%")[0];
			emit SetProgressBar(_threadIndex, progress.toFloat() / 100);
		}
	});
	// Using --no-part because after killing mid-download, .part files stay in use and cant be deleted, removed android from download as it always spits out an error
	_currentProcess->startCommand(QString(R"("%1" --no-part --extractor-args youtube:player_client=ios,web -f m4a/bestaudio/best -o "%2" --ffmpeg-location "%3" -x --audio-quality 0 --audio-format %4 "%5")")
						.arg(QCoreApplication::applicationDirPath() + "/" + YTDLP_PATH)
						.arg(downloadingPath)
						.arg(QCoreApplication::applicationDirPath() + "/" + FFMPEG_PATH)
						.arg(CODEC)
						.arg(QString("https://www.youtube.com/watch?v=%1").arg(finalResult["videoId"].toString())));
	_currentProcess->waitForFinished(-1);

	emit SetProgressBar(_threadIndex, 1);
	#pragma endregion

	if (_quitting) return;
	CheckForStop();

	#pragma region Set Audio Quality
	// Audio quality will be somewhere around 256kb/s (not exactly) so change it to desired quality ranging from 33 - 256 in this case (33 is minimum, from there 64, 96, 128, ...)
	// If quality is not a multiple of 32 ffmpeg will change it to the closest multiple
	// Don't set quality if normalizing, has to be set there regardless

	if (!Main->NormalizeAudio) {
		emit SetProgressLabel(_threadIndex, "Setting Bitrate...");

		_currentProcess = new QProcess();
		connect(_currentProcess, &QProcess::finished, _currentProcess, &QProcess::deleteLater);

		QString newQualityFullPath = QString("%1/%2.%3").arg(downloadingFolder).arg(fileName + "_A").arg(CODEC);
		_currentProcess->startCommand(QString(R"("%1" -i "%2"  -b:a %3k "%4")")
						.arg(QCoreApplication::applicationDirPath() + "/" + FFMPEG_PATH)
						.arg(downloadingPath)
						.arg(Main->AudioBitrate)
						.arg(newQualityFullPath));
		_currentProcess->waitForFinished(-1);

		QFile::remove(downloadingPath);
		QFile::rename(newQualityFullPath, downloadingPath);
	}

	#pragma endregion

	if (_quitting) return;
	CheckForStop();

	#pragma region Normalize Audio
	if (Main->NormalizeAudio) {
		emit SetProgressLabel(_threadIndex, "Normalizing Audio...");

		// Get Audio Data
		_currentProcess = new QProcess();
		connect(_currentProcess, &QProcess::finished, _currentProcess, &QProcess::deleteLater);
		_currentProcess->startCommand(QString(R"("%1" -i "%2" -af "volumedetect" -vn -sn -dn -f null -)")
						.arg(QCoreApplication::applicationDirPath() + "/" + FFMPEG_PATH)
						.arg(downloadingPath));
		_currentProcess->waitForFinished(-1);

		// For some reason ffmpeg outputs to StandardError. idk why
		QString audioOutput = _currentProcess->readAllStandardError();
		if (audioOutput.contains("mean_volume:")) {
			QString normalizedFullPath = QString("%1/%2.%3").arg(downloadingFolder).arg(fileName + "_N").arg(CODEC);
			
			float meanVolume = audioOutput.split("mean_volume:")[1].split("dB")[0].toFloat();

			if (meanVolume != Main->NormalizeAudioVolume) {
				float volumeApply = (Main->NormalizeAudioVolume - meanVolume) + 0.4; // Adding 0.4 here since normalized is always average 0.4-0.5 off of normalized target IDK why (ffmpeg all over the place)

				_currentProcess = new QProcess();
				connect(_currentProcess, &QProcess::finished, _currentProcess, &QProcess::deleteLater);
				_currentProcess->startCommand(QString(R"("%1" -i "%2" -b:a %3k -af "volume=%4dB" "%5")")
								.arg(QCoreApplication::applicationDirPath() + "/" + FFMPEG_PATH)
								.arg(downloadingPath)
								.arg(Main->AudioBitrate)
								.arg(volumeApply)
								.arg(normalizedFullPath));
				_currentProcess->waitForFinished(-1);

				if (_quitting) return;

				QFile::remove(downloadingPath);
				QFile::rename(normalizedFullPath, downloadingPath);
			}
		}
	}
	#pragma endregion

	if (_quitting) return;
	CheckForStop();

	#pragma region Assign Metadata
	emit SetProgressLabel(_threadIndex, "Assigning Metadata...");

	// FileRef destroys when leaving scope, give it a scope to do its thing
	{
		TagLib::FileName tagFileName(reinterpret_cast<const wchar_t*>(downloadingPath.constData()));
		TagLib::FileRef tagFileRef(tagFileName, true, TagLib::AudioProperties::Accurate);
		TagLib::MPEG::File* file = dynamic_cast<TagLib::MPEG::File*>(tagFileRef.file());

		TagLib::ID3v2::Tag* tag = file->ID3v2Tag(true);
		tag->setTitle(reinterpret_cast<const wchar_t*>(trackTitle.constData()));
		tag->setArtist(reinterpret_cast<const wchar_t*>(songArtistNames.constData()));
		tag->setAlbum(reinterpret_cast<const wchar_t*>(albumName.constData()));

		TagLib::ID3v2::TextIdentificationFrame* pubFrame = new TagLib::ID3v2::TextIdentificationFrame("TPUB");
		TagLib::ID3v2::TextIdentificationFrame* copFrame = new TagLib::ID3v2::TextIdentificationFrame("TCOP");
		TagLib::ID3v2::CommentsFrame* comFrame = new TagLib::ID3v2::CommentsFrame();
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
		buffer.close();

		TagLib::ID3v2::AttachedPictureFrame* picFrame = new TagLib::ID3v2::AttachedPictureFrame();
		picFrame->setPicture(TagLib::ByteVector(imageBytes.data(), imageBytes.count()));
		picFrame->setMimeType("image/png");
		picFrame->setType(TagLib::ID3v2::AttachedPictureFrame::FrontCover);
		tag->addFrame(picFrame);

		tagFileRef.save();
	}
	#pragma endregion

	// Remove from target folder if file exists
	if (Main->Overwrite && QFile::exists(targetPath))
		QFile::remove(targetPath);

	// Create target folder if it doesnt exist
	if (!QDir(targetFolder).exists())
		QDir().mkdir(targetFolder);

	// Move from downloading path to target path
	QFile::rename(downloadingPath, targetPath);
}

int SongDownloader::SongsRemaining() {
	// Account for currently downloading song
	int remaining = _totalSongCount - 1 - SongsDownloaded;
	return std::max(remaining, 0);
}

void SongDownloader::AddTracks(QJsonArray tracks) {
	_downloadingTracks = JSONUtils::Extend(_downloadingTracks, tracks);

	int prevTotalSongCount = _totalSongCount;
	_totalSongCount = _downloadingTracks.count();

	emit SetSongCount(_threadIndex, SongsDownloaded + 1, _totalSongCount);
}

QJsonArray SongDownloader::RemoveTracks(int numTracksToRemove) {

	QJsonArray removedTracks = QJsonArray();
	int targetSongCount = _totalSongCount - numTracksToRemove;
	for (int i = _totalSongCount - 1; i >= targetSongCount; i--) {
		QJsonObject track = _downloadingTracks[i].toObject();
		removedTracks.append(track);
		_downloadingTracks.removeAt(i);
	}

	_totalSongCount = targetSongCount;
	emit SetSongCount(_threadIndex, SongsDownloaded + 1, _totalSongCount);

	return removedTracks;
}

void SongDownloader::FinishedDownloading(bool finished) {
	_waitingForFinishedResponse = false;
	_finishedDownloading = finished;
}

void SongDownloader::CheckForStop() {
	// Paused
	if (!Main->Paused) return;

	emit HidePauseWarning(_threadIndex);
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

void SongDownloader::Quit() {
	// Kill the current process, no need to wait
	if (_currentProcess && _currentProcess->state() != QProcess::NotRunning)
		_currentProcess->kill();

	_quitting = true;
}

SongDownloader::~SongDownloader() {
	emit CleanedUp(_threadIndex);
}