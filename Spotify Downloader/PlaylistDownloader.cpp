#include "SpotifyDownloader.h"

void PlaylistDownloader::DownloadSongs(const SpotifyDownloader* main) {
	qDebug() << "downloading";

	Main = main;

	QThread* thread = QThread::currentThread();
	//_threads = QList<Worker*>();

	//emit SetProgressLabel("Getting Playlist Data...");
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

	int targetThreadCount = Main->ThreadCount;

	int threadCount = _totalSongCount < targetThreadCount ? _totalSongCount : targetThreadCount;
	int baseCount = _totalSongCount / threadCount;
	int lastCount = _totalSongCount % threadCount;

	qDebug() << threadCount;

	qDebug() << "downloading";

	QList<QJsonArray> trackList = QList<QJsonArray>();
	for (int i = 0; i < threadCount; i++) {
		int currentStart = i * baseCount;
		int currentCount = i < threadCount - 1 ? baseCount : baseCount + lastCount;

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

	SetupThreads(trackList, album);
	
	//for (int i = 0; i < threadCount; i++) {
	//	//Worker* worker = _threads[i];
	//
	//	//qDebug() << i;
	//	//qDebug() << worker->Downloader;
	//	//
	//	//qDebug() << "starting";
	//	//
	//	//worker->Thread.start();
	//	//worker->Start(Main, _yt, trackList[i], album);
	//
	//	//qDebug() << "ended";
	//}

	//_threads[0]->Thread.start();
	//_threads[0]->Start(Main);

	//for (int i = 0; i < _totalSongCount; i++) {
	//	QJsonObject track = tracks[i].toObject();
	//	if (url.contains("playlist")) track = track["track"].toObject();
	//
	//	_currentTrack = track;
	//	DownloadSong(track, i, album);
	//
	//	if (_quitting) {
	//		this->thread()->quit();
	//		return;
	//	}
	//}

	//delete _yt;
	//delete _sp;
	//
	//if (_tracksNotFound.count() == 0) {
	//	emit ChangeScreen(2);
	//	emit ShowMessage("Downloads Complete!", "No download errors!");
	//	return;
	//}
	//
	//emit ChangeScreen(3);
	//emit ShowMessage("Downloads Complete!", QString("%1 download error(s)...").arg(_tracksNotFound.count()));
	//emit SetErrorItems(_tracksNotFound);
}

void PlaylistDownloader::SetupThreads(QList<QJsonArray> tracks, QJsonObject album) {
	
	qDebug() << "setting up";
	emit SetupUI(tracks.count());
	
	qDebug() << "waiting";
	//QThread::sleep(5);
	qDebug() << "starting back up again";

	for (int i = 0; i < tracks.count(); i++) {
		qDebug() << "next";
		Worker* worker = new Worker();

		//SongDownloader* downloader = new SongDownloader();
		//QThread thread;

		// Get thread ready to be started
		worker->Downloader = new SongDownloader();
		worker->Downloader->moveToThread(&worker->Thread);
		connect(&worker->Thread, &QThread::finished, worker->Downloader, &QObject::deleteLater);
		connect(this, &PlaylistDownloader::DownloadOnThread, worker->Downloader, &SongDownloader::DownloadSongs);

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

		qDebug() << "downloading";

		worker->Thread.start();

		qDebug() << "emit";
		emit DownloadOnThread(Main, _yt, tracks[i], album, i);

		qDebug() << "disconnect";
		disconnect(this, &PlaylistDownloader::DownloadOnThread, worker->Downloader, &SongDownloader::DownloadSongs);

		//connect(this, &PlaylistDownloader::StartDownloading, thread, &Worker::Start);

		//_threads << thread;

		qDebug() << "done with this one";
	}

	qDebug() << "done";
}