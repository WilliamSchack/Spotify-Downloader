#include "SpotifyDownloader.h"

void PlaylistDownloader::DownloadSongs(const SpotifyDownloader* main) {
	Main = main;

	QThread* thread = QThread::currentThread();

	emit ShowMessage("Starting Download!", "This may take a while...");

	QString url = Main->PlaylistURLText;
	QString spotifyId = url.split("/").last().split("?")[0];
	QJsonArray tracks = QJsonArray();
	QJsonObject album = QJsonObject();

	_yt = new YTMusicAPI();
	_sp = new SpotifyAPI();

	if (url.contains("playlist")) tracks = _sp->GetPlaylistTracks(spotifyId);
	else if (url.contains("album")) {
		album = _sp->GetAlbum(spotifyId);
		tracks = _sp->GetAlbumTracks(album);
	}
	else tracks = QJsonArray{ _sp->GetTrack(spotifyId) };

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
	qDebug() << "Request recieved";

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
}