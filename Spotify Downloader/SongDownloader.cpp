#include "SpotifyDownloader.h"

#include "Song.h"

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

	_tracksNotFound = QJsonArray();
	for (int i = startIndex; i < _totalSongCount; i++) {
		QJsonObject track = _downloadingTracks[i].toObject();

		qInfo() << "Thread" << _threadIndex << "starting download track" << i << "/" << _totalSongCount << track;

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
	if (_finishedDownloading) {
		qInfo() << "Finished downloading on thread" << _threadIndex;
		return;
	}

	// If songs are added restart the downloading from the new index
	StartDownload(startingTotalSongCount);
}

void SongDownloader::DownloadSong(QJsonObject track, int count, QJsonObject album) {
	if (album.isEmpty()) album = track["album"].toObject();

	// Initialise Song
	Song song = Song(track, album, YTDLP_PATH, FFMPEG_PATH, CODEC, Main);
	qInfo() << _threadIndex << "Initialised song" << song.SpotifyId;

	// Set target folder
	QString targetFolderName = "";
	switch (Main->FolderSortingIndex) {
		case 1: // Album name
			targetFolderName = song.AlbumName;
			break;
		case 2: // Song Artist
			targetFolderName = song.ArtistName;
			break;
		case 3: // Song Artists
			for (int i = 0; i < song.ArtistNamesList.count(); i++) {
				QString artistName = song.ArtistNamesList[i];
				targetFolderName.append(artistName);
				if (i < song.ArtistNamesList.count() - 1)
					targetFolderName.append(", ");
			}
			break;
		case 4: // Album Artist
			targetFolderName = song.AlbumArtistNamesList[0];
			break;
		case 5: // Album Artists
			for (int i = 0; i < song.AlbumArtistNamesList.count(); i++) {
				QString artistName = song.AlbumArtistNamesList[i];
				targetFolderName.append(artistName);
				if (i < song.AlbumArtistNamesList.count() - 1)
					targetFolderName.append(", ");
			}
			break;
	}
	
	// Set target path
	QString targetFolder = QString("%1/%2").arg(Main->SaveLocationText).arg(targetFolderName);
	QString targetPath = QString("%1/%2.%3").arg(targetFolder).arg(song.FileName).arg(CODEC);

	// If not overwriting and song already downloaded, skip to next song
	if (!Main->Overwrite && QFile::exists(targetPath)) {
		qInfo() << _threadIndex << "Song" << song.SpotifyId << "already downloaded, skipping...";
		return;
	}

	// Check for quit/pause
	if (_quitting) return;
	CheckForStop();

	// Download cover image
	qInfo() << _threadIndex << "Downloading cover art for" << song.SpotifyId;
	emit SetProgressLabel(_threadIndex, "Downloading Cover Art...");
	song.DownloadCoverImage();
	qInfo() << _threadIndex << "Cover art downloaded for" << song.SpotifyId;

	// Check for quit/pause
	if (_quitting) return;
	CheckForStop();

	// Assign data to GUI
	emit SetSongImage(_threadIndex, QPixmap::fromImage(song.CoverImage));
	emit SetSongDetails(_threadIndex, song.Title, song.ArtistNames.replace(";", ","));
	emit SetSongCount(_threadIndex, count + 1, _totalSongCount);

	// Check for quit/pause
	if (_quitting) return;
	CheckForStop();

	// Search for song
	qInfo() << _threadIndex << "Searching for" << song.SpotifyId;
	emit SetProgressLabel(_threadIndex, "Searching...");
	bool resultFound = song.SearchForSong(_yt);
	if (!resultFound) {
		emit SetProgressLabel(_threadIndex, "SONG NOT FOUND");
		_tracksNotFound.append(QJsonObject{
			{"title", song.Title},
			{"album", song.AlbumName},
			{"artists", song.ArtistNames},
			{"image", JSONUtils::PixmapToJSON(QPixmap::fromImage(song.CoverImage))}
		});
	
		qWarning() << _threadIndex << "Could not find search result for" << song.SpotifyId;

		QThread::sleep(2);
		return;
	}
	qInfo() << _threadIndex << QString("Found search result (%1) for").arg(song.YoutubeId) << song.SpotifyId;

	// Check for quit/pause
	if (_quitting) return;
	CheckForStop();

	// Download song
	qInfo() << _threadIndex << "Downloading song" << song.SpotifyId;
	emit SetProgressLabel(_threadIndex, "Downloading Track...");
	emit SetProgressBar(_threadIndex, 0);
	song.Download(_currentProcess, Main->Overwrite, [&]() {
		QString output = _currentProcess->readAll();
		if (output.contains("[download]") && !output.contains(song.FileName)) { // Make sure that it is a download status output and not another file related thing
			QString progress = output.split("]")[1].split("%")[0].replace(" ", "");
			emit SetProgressBar(_threadIndex, progress.toFloat() / 100);
		}
	});

	qInfo() << _threadIndex << "Successfully downloaded song" << song.SpotifyId;

	emit SetProgressBar(_threadIndex, 1);

	// Check for quit/pause
	if (_quitting) return;
	CheckForStop();

	// If normalising, normalise audio, includes setting bitrate of audio
	if (Main->NormalizeAudio) {
		qInfo() << _threadIndex << "Normalising audio for song" << song.SpotifyId;
		emit SetProgressLabel(_threadIndex, "Normalizing Audio...");
		song.NormaliseAudio(_currentProcess, Main->NormalizeAudioVolume, Main->AudioBitrate, &_quitting);
		qInfo() << _threadIndex << "Successfully normalised audio for song" << song.SpotifyId;
	}
	// Otherwise set the bitrate
	else {
		qInfo() << _threadIndex << "Setting bitrate for song" << song.SpotifyId;
		emit SetProgressLabel(_threadIndex, "Setting Bitrate...");
		song.SetBitrate(_currentProcess, Main->AudioBitrate);
		qInfo() << _threadIndex << "Successfully set bitrate for song" << song.SpotifyId;
	}

	// Check for quit/pause
	if (_quitting) return;
	CheckForStop();

	// Assign metadata
	qInfo() << _threadIndex << "Assigning metadata for song" << song.SpotifyId;
	emit SetProgressLabel(_threadIndex, "Assigning Metadata...");
	song.AssignMetadata();

	song.Save(targetFolder, targetPath, Main->Overwrite);

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

void SongDownloader::CheckForStop() {
	// Paused
	if (!Main->Paused) return;

	qInfo() << "Thread" << _threadIndex << "paused...";

	emit HidePauseWarning(_threadIndex);
	while (Main->Paused) {
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