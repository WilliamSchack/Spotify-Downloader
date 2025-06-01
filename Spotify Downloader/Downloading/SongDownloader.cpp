#include "SpotifyDownloader.h"

void SongDownloader::DownloadSongs(const SpotifyDownloader* main, const PlaylistDownloader* manager, YTMusicAPI* yt, QJsonArray tracks, QJsonObject album, int threadIndex) {
	Main = main;
	Manager = manager;
	_yt = yt;
	_downloadingTracks = tracks;
	_album = album;
	_threadIndex = threadIndex;
	_totalSongCount = _downloadingTracks.count();

	emit SetProgressLabel(_threadIndex, "Getting Playlist Data...");

	qInfo() << "Started downloading on thread" << _threadIndex;

	StartDownload(0);
}

void SongDownloader::StartDownload(int startIndex) {
	int startingTotalSongCount = _totalSongCount;

	_downloadErrors = QJsonArray();
	for (int i = startIndex; i < _totalSongCount; i++) {
		QJsonObject track = _downloadingTracks[i].toObject();

		qInfo() << "Thread" << _threadIndex << "starting download track" << i << "/" << _totalSongCount << track["id"].toString();

		_currentTrack = track;
		DownloadSong(track, i, _album);

		if (_quitting) {
			// Add download errors before quitting
			emit AddDownloadErrors(_threadIndex, _downloadErrors);

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
	emit Finish(_threadIndex, _downloadErrors);
	while (_waitingForFinishedResponse) {
		QCoreApplication::processEvents();
	}

	// If no songs are added and done downloading on thread finish up
	if (_finishedDownloading) {
		qInfo() << "Finished downloading on thread" << _threadIndex;
		return;
	}

	// If songs are added restart the downloading from the new index
	StartDownload(startingTotalSongCount);
}

void SongDownloader::DownloadSong(QJsonObject track, int count, QJsonObject album) {
	if (album.isEmpty()) album = track["album"].toObject();

	// Check for quit/pause
	CheckForStop();
	if (_quitting) return;

	// Codec can be changed during download, use codec set at start of download
	Codec::Extension codec = Config::Codec;

	// Initialise Song
	Song song = Song(track, album, Config::YTDLP_PATH, Config::FFMPEG_PATH, codec, Main);
	qInfo() << _threadIndex << "Initialised song" << song.SpotifyId;

	emit SetSongDetails(_threadIndex, song.Title, song.ArtistNamesList.join(", "));
	emit SetSongCount(_threadIndex, count + 1, _totalSongCount);
	emit SetProgressBar(_threadIndex, 0, 0);

	// Set target folder
	QString targetFolderName = "/";
	if (!Config::SubFolders.isEmpty()) {
		targetFolderName = QString("/%1").arg(std::get<0>(Song::SubFoldersWithTags(song)));
		targetFolderName.replace("\\", "/");
		targetFolderName = StringUtils::ValidateFolderName(targetFolderName);
	}
	
	// Set target path
	QString targetFolder = QString("%1%2").arg(Config::SaveLocation).arg(targetFolderName);
	QString targetPath = QString("%1/%2.%3").arg(targetFolder).arg(song.FileName).arg(Codec::Data[codec].String);

	// If not overwriting and song already downloaded, skip to next song
	if (!Config::Overwrite && QFile::exists(targetPath)) {
		qInfo() << _threadIndex << "Song" << song.SpotifyId << "already downloaded, skipping...";
		return;
	}

	// Check for quit/pause
	CheckForStop();
	if (_quitting) return;
	
	// Download cover image
	qInfo() << _threadIndex << "Downloading cover art for" << song.SpotifyId;
	emit SetProgressLabel(_threadIndex, "Downloading Cover Art...");
	song.DownloadCoverImage();
	emit SetSongImage(_threadIndex, song.CoverImage);
	emit SetProgressBar(_threadIndex, 0.1);
	qInfo() << _threadIndex << "Cover art downloaded for" << song.SpotifyId;

	// Check for quit/pause
	CheckForStop();
	if (_quitting) return;

	// Check for quit/pause
	CheckForStop();
	if (_quitting) return;

	// Search for song
	qInfo() << _threadIndex << "Searching for" << song.SpotifyId;
	emit SetProgressLabel(_threadIndex, "Searching...");
	QString searchResult = song.SearchForSong(_yt, [=](float percentComplete) {
		emit SetProgressBar(_threadIndex, MathUtils::Lerp(0.1, 0.3, percentComplete));
	});

	// If result contains an error, add it to the download errors
	if (!searchResult.isEmpty()) {
		AddSongToErrors(song, searchResult);

		// If IP is flagged cancel download and warn user in download errors
		if (searchResult.contains("YouTube has flagged your IP")) {
			emit RequestQuit();
		}

		return;
	}
	qInfo() << _threadIndex << QString("Found search result (%1) for").arg(song.YoutubeId) << song.SpotifyId;

	// Check for quit/pause
	CheckForStop();
	if (_quitting) return;

	// Download song
	int bitrateOverride = -1;

	qInfo() << _threadIndex << "Downloading song" << song.SpotifyId;
	emit SetProgressLabel(_threadIndex, "Downloading Track...");

	QString downloadResult = song.Download(_yt, _currentProcess, Config::Overwrite,
		// On Progress Update
		[&](float percentComplete) {
			float progressBarPercent = MathUtils::Lerp(0.3, 0.7, percentComplete);
			emit SetProgressBar(_threadIndex, progressBarPercent);
		},
		// On PO Token Warning
		[this]() {
			emit ShowPOTokenError();
		},
		// On Low Quality Warning
		[&]() {
			// Only show warning if bitrate set to premium quality
			int maxNonPremiumBitrate = Codec::Data[Config::Codec].MaxBitrate;
			if (Config::AudioBitrate[codec] <= maxNonPremiumBitrate)
				return;

			AddSongToErrors(song, "Song does not have a 256kb/s version, downloaded at 128kb/s", true);

			// Premium already verified, set bitrate override to non-premium max to not waste storage
			bitrateOverride = maxNonPremiumBitrate;
		},
		// On Premium Disabled
		[&]() {
			// Call warning and reset bitrate if higher than non-premium account
			int maxBitrate = Codec::Data[Config::Codec].MaxBitrate;
			// Dont set bitrate if already under or equal to max
			if (Config::GetBitrate() <= maxBitrate) {
				// Warn about expired cookies
				emit ShowMessageBox("Cookies Expired", "Your cookies have expired. If you have not recently reset your cookies, please do so.", QMessageBox::Warning);
				
				// No need to set bitrate
				return;
			}

			// Warn user bitrate has been modified
			emit ShowMessageBox("Using Premium Bitrate or Cookies Expired", "You have assigned a premium bitrate without a YouTube premium account, or your cookies have expired, lowering bitrate down to maximum Non-Premium rate. If you have premium and have not recently reset your cookies, please do so.", QMessageBox::Warning);

			// Set max bitrate and reload UI
			Config::SetBitrate(maxBitrate);
			
			emit LoadSettingsUI();
		}
	);

	if (!downloadResult.isEmpty()) {
		AddSongToErrors(song, downloadResult);

		// Check for cookie errors and quit if any, cannot continue downloading if they have expired
		// Not an elegant way to check for it but it works
		if (downloadResult.contains("Your cookies have expired")) {
			emit SetDownloadStatus("Your cookies have expired\nPlease reset them and the PO Token");
			Manager->thread()->quit();
		}

		return;
	}

	qInfo() << _threadIndex << "Successfully downloaded song" << song.SpotifyId;

	// Check for quit/pause
	CheckForStop();
	if (_quitting) return;

	// If normalising, normalise audio, includes setting bitrate of audio
	int bitrate = bitrateOverride != -1 ? bitrateOverride : Config::AudioBitrate[codec]; // Get current bitrate
	if (Config::NormalizeAudio) {
		qInfo() << _threadIndex << "Normalising audio for song" << song.SpotifyId;
		emit SetProgressLabel(_threadIndex, "Normalizing Audio...");
		song.NormaliseAudio(_currentProcess, Config::NormalizeAudioVolume, bitrate, &_quitting, [&](float percentComplete) {
			float progressBarPercent = MathUtils::Lerp(0.7, 1, percentComplete);
			emit SetProgressBar(_threadIndex, progressBarPercent);
		});
		qInfo() << _threadIndex << "Successfully normalised audio for song" << song.SpotifyId;
	}
	// Otherwise set the bitrate
	else {
		qInfo() << _threadIndex << "Setting bitrate for song" << song.SpotifyId;
		emit SetProgressLabel(_threadIndex, "Setting Bitrate...");
		song.SetBitrate(_currentProcess, bitrate, [&](float percentComplete) {
			float progressBarPercent = MathUtils::Lerp(0.7, 1, percentComplete);
			emit SetProgressBar(_threadIndex, progressBarPercent);
		});
		qInfo() << _threadIndex << "Successfully set bitrate for song" << song.SpotifyId;
	}

	emit SetProgressBar(_threadIndex, 1);

	// Check for quit/pause
	CheckForStop();
	if (_quitting) return;

	// Assign metadata, too quick for progress bar
	qInfo() << _threadIndex << "Assigning metadata for song" << song.SpotifyId;
	emit SetProgressLabel(_threadIndex, "Assigning Metadata...");
	song.AssignMetadata();

	song.Save(targetFolder, targetPath, Config::Overwrite);

	qInfo() << _threadIndex << "Successfully saved song" << song.SpotifyId;
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

void SongDownloader::AddSongToErrors(Song song, QString error, bool silent) {
	_downloadErrors.append(QJsonObject{
		{"id", song.SpotifyId},
		{"title", song.Title},
		{"album", song.AlbumName},
		{"artists", song.ArtistNames},
		{"image", JSONUtils::PixmapToJSON(QPixmap::fromImage(song.CoverImage))},
		{"error", error }
	});

	if (!silent) {
		qWarning() << _threadIndex << "Could not find search result for" << song.SpotifyId << "with the error:" << error;

		emit SetProgressLabel(_threadIndex, "CANNOT DOWNLOAD");
		emit SetProgressBar(_threadIndex, 1);

		QThread::msleep(500);
	}
}

void SongDownloader::CheckForStop() {
	// Paused
	if (!Main->Paused) return;

	qInfo() << "Thread" << _threadIndex << "paused...";

	emit HidePauseWarning(_threadIndex);
	while (Main->Paused && !_quitting) {
		QCoreApplication::processEvents();
	}

	qInfo() << "Thread" << _threadIndex << "unpaused";
}

void SongDownloader::Quit() {
	// Kill the current process, no need to wait
	if (_currentProcess && _currentProcess->state() != QProcess::NotRunning)
		_currentProcess->kill();

	qInfo() << "Thread" << _threadIndex << "quitting...";

	_quitting = true;
}

SongDownloader::~SongDownloader() {
	qInfo() << "Thread" << _threadIndex << "cleaned up";
	emit CleanedUp(_threadIndex);
}