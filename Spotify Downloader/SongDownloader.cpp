#include "SpotifyDownloader.h"

#include "Network.h"

#include <difflib.h>

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
	_threadIndex = threadIndex;
	_totalSongCount = tracks.count();

	QThread* thread = QThread::currentThread();

	emit SetProgressLabel(_threadIndex, "Getting Playlist Data...");
	emit ShowMessage("Starting Download!", "This may take a while...");

	_tracksNotFound = QJsonArray();
	for (int i = 0; i < tracks.count(); i++) {
		QJsonObject track = tracks[i].toObject();

		_currentTrack = track;
		DownloadSong(track, i, album);

		emit SongDownloaded();

		if (_quitting) {
			this->thread()->quit();
			return;
		}
	}

	emit Finish(_threadIndex, _tracksNotFound);
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

	QString filename = QString("%1 - %2").arg(trackTitle).arg(artistName);
	filename = ValidateString(filename);

	QString tempPath = QString("%1/SpotifyDownloader").arg(QDir::temp().path());
	if (!QDir(tempPath).exists()) QDir().mkdir(tempPath);

	QString downloadingFolder = QString("%1/Downloading").arg(tempPath);
	QString downloadingPath = QString("%1/%2").arg(downloadingFolder).arg(filename);
	QString fullDownloadingPath = QString("%1.%2").arg(downloadingPath).arg(CODEC);

	QString targetPath = QString("%1/%2").arg(Main->SaveLocationText).arg(filename);
	QString fullTargetPath = QString("%1.%2").arg(targetPath).arg(CODEC);

	if (!QDir(downloadingFolder).exists()) QDir().mkdir(downloadingFolder);

	if (!Main->Overwrite && QFile::exists(fullTargetPath)) return;
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

	if (Main->Overwrite && QFile::exists(fullDownloadingPath))
		QFile::remove(fullDownloadingPath);

	_currentProcess = new QProcess();
	connect(_currentProcess, &QProcess::finished, _currentProcess, &QProcess::deleteLater);
	connect(_currentProcess, &QProcess::readyRead, _currentProcess, [&]() {
		QString output = _currentProcess->readAll();
		if (output.contains("%")) {
			QString progress = output.split("]")[1].split("%")[0];
			emit SetProgressBar(_threadIndex, progress.toFloat() / 100);
		}
	});
	// Using --no-part because after killing mid-download, .part files stay in use and cant be deleted
	_currentProcess->startCommand(QString(R"("%1" --no-part -f m4a/bestaudio/best -o "%2" --ffmpeg-location "%3" -x --audio-format %4 "%5")")
						.arg(QCoreApplication::applicationDirPath() + "/" + YTDLP_PATH)
						.arg(fullDownloadingPath)
						.arg(QCoreApplication::applicationDirPath() + "/" + FFMPEG_PATH)
						.arg(CODEC)
						.arg(QString("https://www.youtube.com/watch?v=%1").arg(finalResult["videoId"].toString())));
	_currentProcess->waitForFinished(-1);

	emit SetProgressBar(_threadIndex, 1);
	#pragma endregion

	if (_quitting) return;
	CheckForStop();

	#pragma region Normalize Audio
	if (Main->NormalizeAudio) {
		emit SetProgressLabel(_threadIndex, "Normalizing Audio...");

		_currentProcess = new QProcess();
		connect(_currentProcess, &QProcess::finished, _currentProcess, &QProcess::deleteLater);
		_currentProcess->startCommand(QString(R"("%1" -i "%2" -af "volumedetect" -vn -sn -dn -f null -)")
						.arg(QCoreApplication::applicationDirPath() + "/" + FFMPEG_PATH)
						.arg(fullDownloadingPath));
		_currentProcess->waitForFinished(-1);

		// For some reason ffmpeg outputs to StandardError. idk why
		QString audioOutput = _currentProcess->readAllStandardError();
		if (audioOutput.contains("mean_volume:")) {
			QString normalizedFullPath = QString("%1/%2.%3").arg(downloadingFolder).arg(filename + "_N").arg(CODEC);
			
			float meanVolume = audioOutput.split("mean_volume:")[1].split("dB")[0].toFloat();

			if (meanVolume != Main->NormalizeAudioVolume) {
				float volumeApply = (Main->NormalizeAudioVolume - meanVolume) + 0.4; // Adding 0.4 here since normalized is always average 0.4-0.5 off of normalized target IDK why (ffmpeg all over the place)

				_currentProcess = new QProcess();
				connect(_currentProcess, &QProcess::finished, _currentProcess, &QProcess::deleteLater);
				_currentProcess->startCommand(QString(R"("%1" -i "%2" -af "volume=%3" "%4")")
					.arg(QCoreApplication::applicationDirPath() + "/" + FFMPEG_PATH)
					.arg(fullDownloadingPath)
					.arg(QString("%1dB").arg(volumeApply))
					.arg(normalizedFullPath));
				_currentProcess->waitForFinished(-1);

				if (_quitting) return;

				QFile::remove(fullDownloadingPath);
				QFile::rename(normalizedFullPath, fullDownloadingPath);
			}
		}
	}
	#pragma endregion

	if (_quitting) return;
	CheckForStop();

	// Move from downloading path to target path
	// Before metadata because modifying file afterwards doesn't work
	QFile::rename(fullDownloadingPath, fullTargetPath);

	#pragma region Assign Metadata
	emit SetProgressLabel(_threadIndex, "Assigning Metadata...");

	TagLib::MPEG::File file(reinterpret_cast<const wchar_t*>(fullTargetPath.constData()));

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
	if (_currentProcess && _currentProcess->state() != QProcess::NotRunning) {
		_currentProcess->kill();
	}

	_quitting = true;
}

// Cleanup currently downloading songs
SongDownloader::~SongDownloader() {
	if (_currentProcess && _currentProcess->state() != QProcess::NotRunning)
		_currentProcess->waitForFinished();
	emit CleanedUp();
}