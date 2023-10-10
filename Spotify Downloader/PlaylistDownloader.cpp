#include "SpotifyDownloader.h"

void PlaylistDownloader::DownloadSongs(const SpotifyDownloader* main) {
	qDebug() << "downloading";

	Main = main;

	QThread* thread = QThread::currentThread();
	//_threads = QList<Worker*>();

	emit SetProgressLabel("Getting Playlist Data...");
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
	emit SetSongCount(0, _totalSongCount);

	_tracksNotFound = QJsonArray();

	float threadSongCount = (_totalSongCount * 1.0) / 3;
	float firstsecond = std::ceil(threadSongCount);
	float third = std::floor(threadSongCount);

	int threadCount = _totalSongCount < THREAD_COUNT ? _totalSongCount : THREAD_COUNT;
	int baseCount = _totalSongCount / threadCount;
	int lastCount = _totalSongCount % threadCount;

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
	for (int i = 0; i < tracks.count(); i++) {
		Worker* thread = new Worker;

		// Get thread ready to be started
		thread->Downloader = new SongDownloader();
		thread->Downloader->moveToThread(&thread->Thread);
		connect(&thread->Thread, &QThread::finished, thread->Downloader, &QObject::deleteLater);
		connect(this, &PlaylistDownloader::DownloadOnThread, thread->Downloader, &SongDownloader::DownloadSongs);

		// Allow thread to access ui elements
		connect(thread->Downloader, &SongDownloader::ChangeScreen, Main, &SpotifyDownloader::ChangeScreen);
		connect(thread->Downloader, &SongDownloader::ShowMessage, Main, &SpotifyDownloader::ShowMessage);
		connect(thread->Downloader, &SongDownloader::SetProgressLabel, Main, &SpotifyDownloader::SetProgressLabel);
		connect(thread->Downloader, &SongDownloader::SetProgressBar, Main, &SpotifyDownloader::SetProgressBar);
		connect(thread->Downloader, &SongDownloader::SetSongCount, Main, &SpotifyDownloader::SetSongCount);
		connect(thread->Downloader, &SongDownloader::SetSongImage, Main, &SpotifyDownloader::SetSongImage);
		connect(thread->Downloader, &SongDownloader::SetSongDetails, Main, &SpotifyDownloader::SetSongDetails);
		connect(thread->Downloader, &SongDownloader::SetErrorItems, Main, &SpotifyDownloader::SetErrorItems);
		connect(thread->Downloader, &SongDownloader::HidePauseWarning, Main, &SpotifyDownloader::HidePauseWarning);

		thread->Thread.start();

		emit DownloadOnThread(Main, _yt, tracks[i], album);

		disconnect(this, &PlaylistDownloader::DownloadOnThread, thread->Downloader, &SongDownloader::DownloadSongs);

		//connect(this, &PlaylistDownloader::StartDownloading, thread, &Worker::Start);

		//_threads << thread;
	}
}