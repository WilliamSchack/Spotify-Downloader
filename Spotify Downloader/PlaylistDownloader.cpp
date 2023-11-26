#include "SpotifyDownloader.h"

#include <QFileSystemWatcher>
#include <QElapsedTimer>

void PlaylistDownloader::DownloadSongs(const SpotifyDownloader* main) {
	Main = main;

	QThread* thread = QThread::currentThread();

	emit ShowMessage("Starting Download!", "This may take a while...");

	// Remove downloading folder if it exists, cleaning up at end may not clear everything
	QString downloadingFolder = QString("%1/Downloading").arg(QCoreApplication::applicationDirPath());
	if (QDir(downloadingFolder).exists())
		RemoveDir(downloadingFolder);

	QString url = Main->PlaylistURLText;
	QString spotifyId = url.split("/").last().split("?")[0];
	QJsonObject album = QJsonObject();

	_yt = new YTMusicAPI();
	_sp = new SpotifyAPI();

	QJsonArray searchTracks = QJsonArray();
	if (url.contains("playlist")) searchTracks = _sp->GetPlaylistTracks(spotifyId);
	else if (url.contains("album")) {
		album = _sp->GetAlbum(spotifyId);
		searchTracks = _sp->GetAlbumTracks(album);
	}
	else searchTracks = QJsonArray{ _sp->GetTrack(spotifyId) };

	// Remove tracks that already exist if not overwriting
	QJsonArray tracks = QJsonArray();
	if (!Main->Overwrite) {
		QString invalidChars = R"(<>:"/\|?*)";
		foreach(QJsonValue trackVal, searchTracks) {
			QJsonObject track = trackVal.toObject();
			if (url.contains("playlist")) {
				track = track["track"].toObject();
			}

			QString trackTitle = track["name"].toString();
			QString artistName = track["artists"].toArray()[0].toObject()["name"].toString();

			QString filename = QString("%1 - %2").arg(trackTitle).arg(artistName);
			foreach(QChar c, invalidChars) {
				filename.remove(c);
			}

			QString targetPath = QString("%1/%2").arg(Main->SaveLocationText).arg(filename);
			QString fullTargetPath = QString("%1.%2").arg(targetPath).arg(CODEC);

			if (!QFile::exists(fullTargetPath))
				tracks.append(trackVal);
		}
	} else {
		tracks = searchTracks;
	}

	_totalSongCount = tracks.count();
	emit SetSongCount(-1, 0, _totalSongCount);

	_tracksNotFound = QJsonArray();
	_songsDownloaded = 0;
	_threadsFinished = 0;

	int targetThreadCount = Main->ThreadCount;
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
		connect(worker->Downloader, &SongDownloader::Finish, this, &PlaylistDownloader::FinishThread);
		connect(worker->Downloader, &SongDownloader::CleanedUp, this, &PlaylistDownloader::CleanedUp);

		// Allow thread to access ui elements
		connect(worker->Downloader, &SongDownloader::ChangeScreen, Main, &SpotifyDownloader::ChangeScreen);
		connect(worker->Downloader, &SongDownloader::ShowMessage, Main, &SpotifyDownloader::ShowMessage);
		connect(worker->Downloader, &SongDownloader::SetProgressLabel, Main, &SpotifyDownloader::SetProgressLabel);
		connect(worker->Downloader, &SongDownloader::SetProgressBar, Main, &SpotifyDownloader::SetProgressBar);
		connect(worker->Downloader, &SongDownloader::SetSongCount, Main, &SpotifyDownloader::SetSongCount);
		connect(worker->Downloader, &SongDownloader::SetSongImage, Main, &SpotifyDownloader::SetSongImage);
		connect(worker->Downloader, &SongDownloader::SetSongDetails, Main, &SpotifyDownloader::SetSongDetails);
		connect(worker->Downloader, &SongDownloader::SetErrorItems, Main, &SpotifyDownloader::SetErrorItems);
		connect(worker->Downloader, &SongDownloader::HidePauseWarning, Main, &SpotifyDownloader::HidePauseWarning);

		worker->Thread.start();

		emit DownloadOnThread(Main, this, _yt, tracks[i], album, i);
		disconnect(this, &PlaylistDownloader::DownloadOnThread, worker->Downloader, &SongDownloader::DownloadSongs);

		_threads << worker;
	}
}

void PlaylistDownloader::SongDownloaded() {
	_songsDownloaded++;
	emit SetSongCount(-1, _songsDownloaded, _totalSongCount);
}

void PlaylistDownloader::FinishThread(int threadIndex, QJsonArray tracksNotFound) {
	_tracksNotFound = JSONUtils::Extend(_tracksNotFound, tracksNotFound);
	_threadsFinished++;
	_threadsCleaned++;

	emit SetThreadFinished(threadIndex);

	if (_threadsFinished != _threadCount) {
		return;
	}

	delete _yt;
	delete _sp;

	if (_tracksNotFound.count() == 0) {
		emit ChangeScreen(2);
		emit ShowMessage("Downloads Complete!", "No download errors!");
		return;
	}

	emit ChangeScreen(3);
	emit ShowMessage("Downloads Complete!", QString("%1 download error(s)...").arg(_tracksNotFound.count()));
	emit SetErrorItems(_tracksNotFound);
}

void PlaylistDownloader::Quit() {
	_quitting = true;
	this->thread()->quit();
}

void PlaylistDownloader::CleanedUp() {
	_threadsCleaned++;
}

PlaylistDownloader::~PlaylistDownloader() {
	foreach(Worker* worker, _threads) {
		worker->Downloader->Quit();
	}

	while (_threadsCleaned < _threadCount)
		QCoreApplication::processEvents();

	// Remove all files in downloading folder
	QString downloadingFolder = QString("%1/Downloading").arg(QCoreApplication::applicationDirPath());
	RemoveDir(downloadingFolder);
}

void PlaylistDownloader::RemoveDir(const QString& path)
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

	dir.rmdir(path);
}