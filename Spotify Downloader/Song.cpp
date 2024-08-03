#include "Song.h"

Song::Song(QJsonObject song, QJsonObject album, QString ytdlpPath, QString ffmpegPath, const QString codec, const SpotifyDownloader* main) {
	_main = main;

	_ytdlpPath = ytdlpPath;
	_ffmpegPath = ffmpegPath;

	// Get Song Details
	Title = song["name"].toString();
	AlbumName = album["name"].toString();
	ArtistName = song["artists"].toArray()[0].toObject()["name"].toString();
	ArtistsList = song["artists"].toArray();
	AlbumArtistsList = album["artists"].toArray();
	Time = song["duration_ms"].toDouble() / 1000;
	SpotifyId = song["id"].toString();
	AlbumImageURL = album["images"].toArray()[0].toObject()["url"].toString();

	ArtistNamesList = QStringList();
	foreach(QJsonValue artist, ArtistsList) {
		ArtistNamesList.append(artist["name"].toString());
	}
	ArtistNames = ArtistNamesList.join("; ");

	AlbumArtistNamesList = QStringList();
	foreach(QJsonValue artist, AlbumArtistsList) {
		AlbumArtistNamesList.append(artist["name"].toString());
	}
	AlbumArtistNames = AlbumArtistNamesList.join("; ");

	// Generate File Name
	if (_main != nullptr)
		GenerateFileName(_main);

	//Generate Downloading Path
	_codec = codec;
	GenerateDownloadingPath();
}

void Song::GenerateFileName(const SpotifyDownloader* main) {

	// Generate file name, replacing tags for values
	std::tuple<QString, SpotifyDownloader::NamingError> filenameData = main->FormatOutputNameWithTags([=](QString tag) -> QString {
		QStringList namingTags = main->Q_NAMING_TAGS();

		QString tagReplacement = QString();
		int indexOfTag = namingTags.indexOf(tag.toLower());
		switch (indexOfTag) {
		case 0: // Song Name
			tagReplacement = Title;
			break;
		case 1: // Album Name
			tagReplacement = AlbumName;
			break;
		case 2: // Song Artist
			tagReplacement = ArtistName;
			break;
		case 3: // Song Artists
			for (int i = 0; i < ArtistNamesList.count(); i++) {
				QString artistName = ArtistNamesList[i];
				tagReplacement.append(artistName);
				if (i < ArtistNamesList.count() - 1)
					tagReplacement.append(", ");
			}
			break;
		case 4: // Album Artist
			tagReplacement = AlbumArtistNamesList[0];
			break;
		case 5: // Album Artists
			for (int i = 0; i < AlbumArtistNamesList.count(); i++) {
				QString artistName = AlbumArtistNamesList[i];
				tagReplacement.append(artistName);
				if (i < AlbumArtistNamesList.count() - 1)
					tagReplacement.append(", ");
			}
			break;
		case 6: // Song Time Seconds
			tagReplacement = QString::number((int)Time);
			break;
		case 7: // Song Time Minutes
			tagReplacement = QDateTime::fromSecsSinceEpoch(Time, Qt::UTC).toString("mm:ss");
			tagReplacement = tagReplacement.replace(":", "."); // Cannot have colon in file name
			break;
		case 8: // Song Time Hours
			tagReplacement = QDateTime::fromSecsSinceEpoch(Time, Qt::UTC).toString("hh:mm:ss");
			tagReplacement = tagReplacement.replace(":", "."); // Cannot have colon in file name
			break;
		}

		return tagReplacement;
	});

	// No need to check error, was already checked in setup
	FileName = std::get<0>(filenameData);
	FileName = StringUtils::ValidateString(FileName);
}

void Song::GenerateDownloadingPath() {
	_tempPath = QString("%1/SpotifyDownloader").arg(QDir::temp().path());
	if (!QDir(_tempPath).exists())
		QDir().mkdir(_tempPath);

	_downloadingFolder = QString("%1/Downloading").arg(_tempPath);
	_downloadingPath = QString("%1/%2.%3").arg(_downloadingFolder).arg(FileName).arg(_codec);

	if (!QDir(_downloadingFolder).exists())
		QDir().mkdir(_downloadingFolder);
}

void Song::DownloadCoverImage() {
	QString coverFilename = QString("%1(%2)_Cover").arg(AlbumName).arg(AlbumArtistNames);
	coverFilename = StringUtils::ValidateString(coverFilename);
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

bool Song::SearchForSong(YTMusicAPI*& yt) {
	QString searchQuery = QString(R"(%1 - "%2" - %3)").arg(ArtistName).arg(Title).arg(AlbumName);
	QJsonArray searchResults = yt->Search(searchQuery, "songs", 6);
	searchResults = JSONUtils::Extend(searchResults, yt->Search(searchQuery, "videos", 6));

	QJsonArray finalResults = QJsonArray();
	foreach(QJsonValue val, searchResults) {
		QJsonObject result = val.toObject();

		int seconds = result["durationSeconds"].toInt();

		if (seconds != 0 && seconds > Time - 15 && seconds < Time + 15) {
			// Title score
			float totalScore = difflib::MakeSequenceMatcher(result["title"].toString().toStdString(), Title.toStdString()).ratio();

			// Time score
			float timeScore = MathUtils::Lerp(0, 1, (15 - abs(seconds - Time)) / 15);
			totalScore += timeScore;

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
						if (title.toLower().contains(word)) {
							banned = true;
							break;
						}
					}
					if (banned) break;
				}
				if (banned) continue;

				if (timeScore < 0.8 && !hasTitle) continue;

				foreach(QString artist, ArtistNamesList) {
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
					double value = MathUtils::LerpInList(finalViews, i);
					newFinalResults.append(QJsonObject{
						{"result", result["result"].toObject()},
						{"score", result["score"].toDouble() + value},
						{"views", result["viewCount"].toInt()}
						});
				}
				else newFinalResults.append(result);
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
		return false;
	}

	finalResult = finalResult["result"].toObject();
	_searchResult = finalResult;
	YoutubeId = _searchResult["videoId"].toString();
	return true;
}

void Song::Download(QProcess*& process, bool overwrite, std::function<void()> onProgressUpdate) {
	// Remove from temp folder if exists
	if (overwrite && QFile::exists(_downloadingPath))
		QFile::remove(_downloadingPath);

	// Setup Process
	process = new QProcess();
	QObject::connect(process, &QProcess::finished, process, &QProcess::deleteLater);
	QObject::connect(process, &QProcess::readyRead, process, onProgressUpdate);

	// Download song
	// Using --no-part because after killing mid-download, .part files stay in use and cant be deleted, removed android from download as it always spits out an error
	process->startCommand(QString(R"("%1" --no-part --extractor-args youtube:player_client=ios,web -f m4a/bestaudio/best -o "%2" --ffmpeg-location "%3" -x --audio-quality 0 --audio-format %4 "%5")")
		.arg(QCoreApplication::applicationDirPath() + "/" + _ytdlpPath)
		.arg(_downloadingPath)
		.arg(QCoreApplication::applicationDirPath() + "/" + _ffmpegPath)
		.arg(_codec)
		.arg(QString("https://www.youtube.com/watch?v=%1").arg(_searchResult["videoId"].toString())));
	
	process->waitForFinished(-1);
}

void Song::SetBitrate(QProcess*& process, int bitrate) {
	// Audio quality will be somewhere around 256kb/s (not exactly) so change it to desired quality ranging from 33 - 256 in this case (33 is minimum, from there 64, 96, 128, ...)
	// If quality is not a multiple of 32 ffmpeg will change it to the closest multiple
	// Don't set quality if normalizing, has to be set there regardless

	process = new QProcess();
	QObject::connect(process, &QProcess::finished, process, &QProcess::deleteLater);

	QString newQualityFullPath = QString("%1/%2.%3").arg(_downloadingFolder).arg(FileName + "_A").arg(_codec);
	process->startCommand(QString(R"("%1" -i "%2"  -b:a %3k "%4")")
		.arg(QCoreApplication::applicationDirPath() + "/" + _ffmpegPath)
		.arg(_downloadingPath)
		.arg(bitrate)
		.arg(newQualityFullPath));
	process->waitForFinished(-1);

	QFile::remove(_downloadingPath);
	QFile::rename(newQualityFullPath, _downloadingPath);
}

void Song::NormaliseAudio(QProcess*& process, float normalisedAudioVolume, int bitrate, bool* quitting) {
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
		QString normalizedFullPath = QString("%1/%2.%3").arg(_downloadingFolder).arg(FileName + "_N").arg(_codec);

		float meanVolume = audioOutput.split("mean_volume:")[1].split("dB")[0].toFloat();

		if (meanVolume != normalisedAudioVolume) {
			float volumeApply = (normalisedAudioVolume - meanVolume) + 0.4; // Adding 0.4 here since normalized is always average 0.4-0.5 off of normalized target IDK why (ffmpeg all over the place)

			process = new QProcess();
			QObject::connect(process, &QProcess::finished, process, &QProcess::deleteLater);
			process->startCommand(QString(R"("%1" -i "%2" -b:a %3k -af "volume=%4dB" "%5")")
				.arg(QCoreApplication::applicationDirPath() + "/" + _ffmpegPath)
				.arg(_downloadingPath)
				.arg(bitrate)
				.arg(volumeApply)
				.arg(normalizedFullPath));
			process->waitForFinished(-1);

			if (quitting) return;

			QFile::remove(_downloadingPath);
			QFile::rename(normalizedFullPath, _downloadingPath);
		}
	}
}

void Song::AssignMetadata() {
	// FileRef destroys when leaving scope, give it a scope to do its thing
	{
		TagLib::FileName tagFileName(reinterpret_cast<const wchar_t*>(_downloadingPath.constData()));
		TagLib::FileRef tagFileRef(tagFileName, true, TagLib::AudioProperties::Accurate);
		TagLib::MPEG::File* file = dynamic_cast<TagLib::MPEG::File*>(tagFileRef.file());

		TagLib::ID3v2::Tag* tag = file->ID3v2Tag(true);
		tag->setTitle(reinterpret_cast<const wchar_t*>(Title.constData()));
		tag->setArtist(reinterpret_cast<const wchar_t*>(ArtistNames.constData()));
		tag->setAlbum(reinterpret_cast<const wchar_t*>(AlbumName.constData()));

		TagLib::ID3v2::TextIdentificationFrame* pubFrame = new TagLib::ID3v2::TextIdentificationFrame("TPUB");
		TagLib::ID3v2::TextIdentificationFrame* copFrame = new TagLib::ID3v2::TextIdentificationFrame("TCOP");
		TagLib::ID3v2::CommentsFrame* comFrame = new TagLib::ID3v2::CommentsFrame();
		pubFrame->setText("William S - Spotify Downloader");
		copFrame->setText(QString("Spotify ID (%1), Youtube ID (%2)").arg(SpotifyId).arg(YoutubeId).toStdString().data());
		comFrame->setText("Thanks for using my program! :)\n- William S");
		tag->addFrame(pubFrame);
		tag->addFrame(copFrame);
		tag->addFrame(comFrame);

		QByteArray imageBytes;
		QBuffer buffer(&imageBytes);
		buffer.open(QIODevice::WriteOnly);
		CoverImage.save(&buffer, "PNG");
		buffer.close();

		TagLib::ID3v2::AttachedPictureFrame* picFrame = new TagLib::ID3v2::AttachedPictureFrame();
		picFrame->setPicture(TagLib::ByteVector(imageBytes.data(), imageBytes.count()));
		picFrame->setMimeType("image/png");
		picFrame->setType(TagLib::ID3v2::AttachedPictureFrame::FrontCover);
		tag->addFrame(picFrame);

		tagFileRef.save();
	}
}

void Song::Save(QString targetFolder, QString targetPath, bool overwrite) {
	// Remove from target folder if file exists
	if (overwrite && QFile::exists(targetPath))
		QFile::remove(targetPath);

	// Create target folder if it doesnt exist
	if (!QDir(targetFolder).exists())
		QDir().mkdir(targetFolder);

	// Move from downloading path to target path
	QFile::rename(_downloadingPath, targetPath);
}