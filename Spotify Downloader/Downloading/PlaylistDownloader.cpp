#include "SpotifyDownloader.h"

#include "Downloading/Song.h"

#include "Utilities/StringUtils.h"

#include <QFileSystemWatcher>
#include <QElapsedTimer>

void PlaylistDownloader::DownloadSongs(const SpotifyDownloader* main) {
	Main = main;

	QThread* thread = QThread::currentThread();

	// Remove downloading folder if it exists, cleaning up at end may not clear everything
	QString downloadingFolder = QString("%1/SpotifyDownloader/Downloading").arg(QDir::temp().path());
	if (QDir(downloadingFolder).exists())
		ClearDirFiles(downloadingFolder);

	QString url = Config::PlaylistURL;
	QString spotifyId = url.split("/").last().split("?")[0];
	QJsonObject album = QJsonObject();

	_yt = new YTMusicAPI();
	_sp = new SpotifyAPI();

	// Check connection to servers
	bool ytConnected = _yt->CheckConnection();
	bool spConnected = _sp->CheckConnection();
	if (!ytConnected || !spConnected) {
		QString errorMessage = "Connection To ";
		if (!spConnected) {
			errorMessage.append("Spotify");
			if (!ytConnected)
				errorMessage.append(" & Youtube");
		} else {
			errorMessage.append("Youtube");
		}
		errorMessage.append(" Failed");

		emit ShowMessage("Network Error", errorMessage);
		emit SetDownloadStatus(errorMessage);

		qWarning() << errorMessage;

		Quit();
		return;
	}

	// Get tracks from spotify
	QJsonArray searchTracks = QJsonArray();
	if (url.contains("playlist")) 
		searchTracks = _sp->GetPlaylistTracks(spotifyId);
	else if (url.contains("album")) {
		album = _sp->GetAlbum(spotifyId);
		searchTracks = _sp->GetAlbumTracks(album);
	}
	else if (url.contains("episode"))
		searchTracks = QJsonArray{ _sp->GetEpisode(spotifyId) };
	else // Track
		searchTracks = QJsonArray{ _sp->GetTrack(spotifyId) };

	// Check if spotify returned anything
	if (searchTracks.isEmpty()) {
		emit ShowMessage("Error fetching songs", "Please try another link or ensure your playlist is public");
		emit SetDownloadStatus(R"(Error fetching songs<br><span style="font-size: 13pt">Please try another link or ensure your playlist is public</span>)");

		qWarning() << "Error downloading songs";

		Quit();
		return;
	}
	
	// If one track exists that is empty, we have been rate limited
	else if (searchTracks.count() == 1 && searchTracks[0].toObject().isEmpty()) {
		emit ShowMessage("App is rate limited", "Use your own API keys in the downloading settings");

		QString helpMessage = SpotifyAPI::ClientID.isEmpty() ? "Use your own api keys in the downloading settings" : "Try changing your API keys or using the default ones";
		emit SetDownloadStatus(QString(R"(App is rate limited<br><span style="font-size: 13pt">%1</span>)").arg(helpMessage));

		qWarning() << "Error downloading songs, app is rate limited";

		Quit();
		return;
	}

	// Dont add tracks that shouldnt be added
	QJsonArray tracks = QJsonArray();
	foreach(QJsonValue trackVal, searchTracks) {
		QJsonObject track = trackVal.toObject();
		if (url.contains("playlist")) track = track["track"].toObject();

		// Check if track exists on spotify
		if (track["id"] == QJsonValue::Null) {
			continue;
		}

		// If not overwriting and track already downloaded, dont download
		if (!Config::Overwrite) {
			// Create song object to generate filename
			Song song = Song(track, album, Config::YTDLP_PATH, Config::FFMPEG_PATH, Config::Codec, Main);

			// Check if file already exists
			QString targetPath = Config::SaveLocation;

			// Get sub directories
			QString targetFolderName = "/";
			if (!Config::SubFolders.isEmpty()) {
				targetFolderName = QString("/%1").arg(std::get<0>(Song::SubFoldersWithTags(song)));
				targetFolderName.replace("\\", "/");
				targetFolderName = StringUtils::ValidateFolderName(targetFolderName);
			}

			targetPath = QString("%1/%2/%3.%4").arg(targetPath).arg(targetFolderName).arg(song.FileName).arg(Codec::Data[Config::Codec].String);

			if (!QFile::exists(targetPath))
				tracks.append(trackVal);
		} else {
			tracks.append(trackVal);
		}
	}

	_totalSongCount = tracks.count();
	emit SetSongCount(-1, 0, _totalSongCount);

	// In the case that tracks inputted were removed, return to setup screen
	if (_totalSongCount == 0) {
		emit ShowMessage("Unable To Pass Initial Check", "Songs are probably already downloaded...");
		emit SetDownloadStatus("Unable To Pass Initial Check, Songs Are Probably Already Downloaded");

		qWarning() << "0 Songs passed check from spotify playlist. Songs are probably already downloaded";

		Quit();
		return;
	}

	qInfo() << "Retrieved" << _totalSongCount << "songs from spotify";

	// Setup Variables
	_downloadErrors = QJsonArray();
	_songsDownloaded = 0;
	_threadsFinished = 0;
	_threadCount = 0;

	// Get amount of threads and songs assigned to each
	int targetThreadCount = Config::ThreadCount;
	_threadCount = _totalSongCount < targetThreadCount ? _totalSongCount : targetThreadCount;
	int baseCount = _totalSongCount / _threadCount;
	int lastCount = _totalSongCount % _threadCount;

	QList<QJsonArray> trackList = QList<QJsonArray>();
	for (int i = 0; i < _threadCount; i++) {
		int currentStart = i * baseCount;
		int currentCount = i < _threadCount - 1 ? baseCount : baseCount + lastCount;

		QJsonArray currentArray = QJsonArray();
		for (int x = 0; x < currentCount; x++) {
			if (url.contains("playlist")) {
				currentArray.append(tracks[currentStart + x].toObject()["track"]);
				continue;
			}

			currentArray.append(tracks[currentStart + x]);
		}

		trackList.append(currentArray);
	}

	emit SetupUI(trackList.count());

	QString startingDownloadMessage = "Shouldn't take too long!";
	if (_totalSongCount > 50) startingDownloadMessage = "This may take a while...";
	emit ShowMessage("Starting Download", startingDownloadMessage);

	SetupThreads(trackList, album);
}

void PlaylistDownloader::SetupThreads(QList<QJsonArray> tracks, QJsonObject album) {
	_threads = QList<Worker*>();

	for (int i = 0; i < tracks.count(); i++) {
		Worker* worker = new Worker();

		// Get thread ready to be started
		worker->Downloader = new SongDownloader();
		worker->Downloader->moveToThread(&worker->Thread);
		connect(&worker->Thread, &QThread::finished, worker->Downloader, &QObject::deleteLater);
		connect(this, &PlaylistDownloader::DownloadOnThread, worker->Downloader, &SongDownloader::DownloadSongs);

		connect(worker->Downloader, &SongDownloader::SongDownloaded, this, &PlaylistDownloader::SongDownloaded);
		connect(worker->Downloader, &SongDownloader::ShowPOTokenError, this, &PlaylistDownloader::ShowPOTokenError);
		connect(worker->Downloader, &SongDownloader::Finish, this, &PlaylistDownloader::FinishThread);
		connect(worker->Downloader, &SongDownloader::CleanedUp, this, &PlaylistDownloader::CleanedUp);
		connect(worker->Downloader, &SongDownloader::AddDownloadErrors, this, &PlaylistDownloader::AddDownloadErrors);
		connect(worker->Downloader, &SongDownloader::RequestQuit, this, &PlaylistDownloader::Quit);

		// Allow thread to access ui elements
		connect(worker->Downloader, &SongDownloader::ChangeScreen, Main, &SpotifyDownloader::ChangeScreen);
		connect(worker->Downloader, &SongDownloader::ShowMessage, Main, &SpotifyDownloader::ShowMessage);
		connect(worker->Downloader, &SongDownloader::ShowMessageBox, Main, &SpotifyDownloader::ShowMessageBox);
		connect(worker->Downloader, &SongDownloader::ShowMessageBoxWithButtons, Main, &SpotifyDownloader::ShowMessageBoxWithButtons);
		connect(worker->Downloader, &SongDownloader::SetDownloadStatus, Main, &SpotifyDownloader::SetDownloadStatus);
		connect(worker->Downloader, &SongDownloader::SetProgressLabel, Main, &SpotifyDownloader::SetProgressLabel);
		connect(worker->Downloader, &SongDownloader::SetProgressBar, Main, &SpotifyDownloader::SetProgressBar);
		connect(worker->Downloader, &SongDownloader::SetSongCount, Main, &SpotifyDownloader::SetSongCount);
		connect(worker->Downloader, &SongDownloader::SetSongImage, Main, &SpotifyDownloader::SetSongImage);
		connect(worker->Downloader, &SongDownloader::SetSongDetails, Main, &SpotifyDownloader::SetSongDetails);
		connect(worker->Downloader, &SongDownloader::SetErrorItems, Main, &SpotifyDownloader::SetErrorItems);
		connect(worker->Downloader, &SongDownloader::LoadSettingsUI, Main, &SpotifyDownloader::LoadSettingsUI);
		connect(worker->Downloader, &SongDownloader::HidePauseWarning, Main, &SpotifyDownloader::HidePauseWarning);

		worker->Thread.start();

		emit DownloadOnThread(Main, this, _yt, tracks[i], album, i);
		disconnect(this, &PlaylistDownloader::DownloadOnThread, worker->Downloader, &SongDownloader::DownloadSongs);

		_threads << worker;

		// Log all track ids
		QList<QString> trackIds = QList<QString>();
		for (int x = 0; x < tracks[i].count(); x++) {
			trackIds.append(tracks[i][x].toObject()["id"].toString());
		}

		qInfo() << "Setup thread" << i << "with the songs:" << trackIds;
	}
}

void PlaylistDownloader::SongDownloaded() {
	_songsDownloaded++;
	emit SetSongCount(-1, _songsDownloaded, _totalSongCount);
}

void PlaylistDownloader::ShowPOTokenError() {
	// Dont show error if shown before
	if (_poTokenErrorShown)
		return;

	emit ShowMessageBox("Invalid PO Token", "The assigned PO Token is invalid/expired. Download will continue but cookies will not be used. Please change your PO Token.", QMessageBox::Warning);
	_poTokenErrorShown = true;
}

void PlaylistDownloader::AddDownloadErrors(int threadIndex, QJsonArray downloadErrors) {
	_downloadErrors = JSONUtils::Extend(_downloadErrors, downloadErrors);
}

void PlaylistDownloader::FinishThread(int threadIndex, QJsonArray downloadErrors) {
	AddDownloadErrors(threadIndex, downloadErrors);

	// If there are still songs remaining across all threads, distribute tracks between them
	int songsLeft = _totalSongCount - (_songsDownloaded + _threadCount - _threadsFinished - 1); // songs left except currently downloading songs
	if (songsLeft > _threadCount - _threadsFinished) {
		if (DistributeTracks()) {
			_threads[threadIndex]->Downloader->FinishedDownloading(false);

			qInfo() << "Redistributed songs to thread" << threadIndex;
			return;
		}
	}

	_threads[threadIndex]->Downloader->FinishedDownloading(true);

	_threadsFinished++;
	_threadsCleaned++;

	emit SetThreadFinished(threadIndex);

	qInfo() << "Thread" << threadIndex << "finished downloading";

	if (_threadsFinished != _threadCount) {
		return;
	}

	Quit();
}

void PlaylistDownloader::DisplayFinalMessage() {
	if (_downloadErrors.count() == 0) {
		emit ShowMessage("Downloads Complete!", "No download errors!");

		return;
	}

	int downloadErrorsCount = _downloadErrors.count();
	emit ShowMessage("Downloads Complete!", QString("%1 download error%2...").arg(downloadErrorsCount).arg(downloadErrorsCount != 1 ? "s" : ""));
}

// Distribute songs evenly between threads based on the remaining songs on each
bool PlaylistDownloader::DistributeTracks() {
	PauseNewDownloads = true;

	// Get the amount of songs that each thread should have after reassigning //
	
	// Get sum of total songs remaining
	int totalTracksRemaining = 0;
	foreach (Worker* currentThread, _threads) {
		SongDownloader* downloader = currentThread->Downloader;
		totalTracksRemaining += downloader->SongsRemaining();
	}
	int tracksRemainingPerThread = totalTracksRemaining / _threadCount;
	int tracksRemainingRemainder = totalTracksRemaining % _threadCount;

	// Prevent stack overflow when there are not enough tracks for redistribution remaining
	if (totalTracksRemaining == 0 || tracksRemainingPerThread == 0) {
		PauseNewDownloads = false;
		return false;
	}

	// Get differences between current total and target total
	QList<int> differenceInTracksPerThread = QList<int>(_threadCount);

	for (int i = 0; i < _threadCount; i++) {
		SongDownloader* downloader = _threads[i]->Downloader;

		int tracksRemaining = downloader->SongsRemaining();
		int targetTrackCount = tracksRemainingPerThread + (i < tracksRemainingRemainder ? 1 : 0);

		int difference = targetTrackCount - tracksRemaining;
		differenceInTracksPerThread[i] = difference;
	}

	// Remove songs from threads that require it //

	QJsonArray removedTracks = QJsonArray();
	for (int i = 0; i < _threadCount; i++) {
		// Difference must be negative
		if (differenceInTracksPerThread[i] >= 0) continue;

		QJsonArray threadRemovedTracks = _threads[i]->Downloader->RemoveTracks(-differenceInTracksPerThread[i]);
		removedTracks = JSONUtils::Extend(removedTracks, threadRemovedTracks);
	}

	// Add songs to threads that require them //

	int currentRemovedIndex = 0;
	for (int i = 0; i < _threadCount; i++) {
		// Difference must be positive
		if (differenceInTracksPerThread[i] <= 0) continue;

		QJsonArray tracksToAdd = QJsonArray();
		int targetIndex = removedTracks.count() - differenceInTracksPerThread[i];
		for (int x = removedTracks.count() - 1; x >= targetIndex; x--) {
			tracksToAdd.append(removedTracks[x]);
			removedTracks.removeAt(x);
		}
		currentRemovedIndex += differenceInTracksPerThread[i];

		_threads[i]->Downloader->AddTracks(tracksToAdd);
	}
	
	PauseNewDownloads = false;

	return true;
}

void PlaylistDownloader::Quit() {
	_quitting = true;
	this->thread()->quit();
}

void PlaylistDownloader::CleanedUp(int threadIndex) {
	emit SetThreadFinished(threadIndex);

	_threadsCleaned++;
}

PlaylistDownloader::~PlaylistDownloader() {
	qInfo() << "Playlist Downloader Quitting";

	foreach(Worker* worker, _threads) {
		worker->Downloader->Quit();
	}

	if (_threadCount > 0) {
		qInfo() << "Waiting for threads to clean...";
		while (_threadsCleaned < _threadCount)
			QCoreApplication::processEvents();
	}

	// Cleanup variables
	if (!Main->ExitingApplication) {
		emit ResetDownloadingVariables();
		while (!Main->VariablesResetting) // Wait for reset to start
			QCoreApplication::processEvents();
		while (Main->VariablesResetting) // Wait for reset to finish
			QCoreApplication::processEvents();
	}

	// Display final message
	emit DisplayFinalMessage();

	// Cleanup
	delete _yt;
	delete _sp;

	// Remove all temp files
	QString tempFolder = QString("%1/SpotifyDownloader").arg(QDir::temp().path());
	QString coverArtFolder = QString("%1/Cover Art").arg(tempFolder);
	QString downloadingFolder = QString("%1/Downloading").arg(tempFolder);

	ClearDirFiles(coverArtFolder);
	ClearDirFiles(downloadingFolder);

	QString cookieFilePath = QString("%1/cookies.txt").arg(tempFolder);
	QFile(cookieFilePath).remove();

	if (!Main->ExitingApplication) {
		// Open download folder
		if (Config::AutoOpenDownloadFolder)
			emit OpenURL(Config::SaveLocation);

		// Change Screen
		if (_downloadErrors.count() == 0) {
			emit ChangeScreen(Config::SETUP_SCREEN_INDEX);
			qInfo() << "Downloads complete with no errors";

			// Save Log after info
			Logger::Flush();

			return;
		}
		else {
			// Set error items
			emit SetErrorItems(_downloadErrors);

			// Change screen
			emit ChangeScreen(Config::ERROR_SCREEN_INDEX);
			
			// Log error items
			QList<QString> trackIds = QList<QString>();
			for (int i = 0; i < _downloadErrors.count(); i++) {
				QJsonObject song = _downloadErrors[i].toObject();

				// Remove image data as its not needed anymore. Can modify variable as its not used later
				song.remove("image");
				_downloadErrors[i] = song;

				// Add song id to log output
				if (song["id"].isNull())
					trackIds.append(QString("ID NULL: (%1 - %2)").arg(song["title"].toString()).arg(song["artists"].toString()));
				else
					trackIds.append(song["id"].toString());
			}

			qInfo() << "Downloads complete with" << _downloadErrors.count() << "error(s)" << trackIds;

			// Save Log after info
			Logger::Flush();
		}

		qInfo() << "Songs downloaded to" << Config::SaveLocation;
	}
}

void PlaylistDownloader::ClearDirFiles(const QString& path)
{
	QStringList failedDirs = QStringList();

	QFileInfo fileInfo(path);
	QDir dir(path);
	QStringList fileList = dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);
	for (int i = 0; i < fileList.count(); ++i) {
		QString curPath = QString("%1/%2").arg(path).arg(fileList[i]);
		QFile curFile(curPath);
		curFile.setPermissions(QFile::WriteOther);
		if (!curFile.remove())
			failedDirs.append(curPath);
	}

	// Sometimes stopping download in between will still keep files being used
	// Wait for those in use files to stop being used then delete
	if (failedDirs.count() > 0) {
		QFileSystemWatcher* watcher = new QFileSystemWatcher(failedDirs);
		foreach(QString string, failedDirs) {
			QFile file(string);
		}

		int completedDeletes = fileList.count() - failedDirs.count();
		QObject::connect(watcher, &QFileSystemWatcher::fileChanged, [&completedDeletes](const QString& path) {
			QFile file(path);
			file.remove();
			if(!file.exists())
				completedDeletes++;
		});

		QElapsedTimer *timer = new QElapsedTimer();
		timer->start(); // Wait 10 secs, if exceeded, cancel waiting for files
		while (completedDeletes < fileList.count() && timer->elapsed() <= 10000)
			QCoreApplication::processEvents();

		delete watcher;
		delete timer;
	}

	qInfo() << "Cleaned directory:" << StringUtils::Anonymize(path);
}

int PlaylistDownloader::DownloadErrors() {
	return _downloadErrors.count();
}