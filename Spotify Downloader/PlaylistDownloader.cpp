#include "SpotifyDownloader.h"

#include "SpotifyAPI.h"

void PlaylistDownloader::DownloadSongs(const SpotifyDownloader* main) {
	Main = main;

	QThread* thread = QThread::currentThread();

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

void PlaylistDownloader::SetupThread() {
	Worker thread;

	// Get thread ready to be started
	thread.SongDownloader = new SongDownloader();
	thread.SongDownloader->moveToThread(&thread.Thread);
	connect(&thread.Thread, &QThread::finished, thread.SongDownloader, &QObject::deleteLater);
	connect(&thread, &Worker::operate, thread.SongDownloader, &SongDownloader::DownloadSongs);

	// Allow thread to access ui elements
	connect(thread.SongDownloader, &SongDownloader::ChangeScreen, this, &PlaylistDownloader::ChangeScreen);
	connect(thread.SongDownloader, &SongDownloader::ShowMessage, this, &PlaylistDownloader::ShowMessage);
	connect(thread.SongDownloader, &SongDownloader::SetProgressLabel, this, &PlaylistDownloader::SetProgressLabel);
	connect(thread.SongDownloader, &SongDownloader::SetProgressBar, this, &PlaylistDownloader::SetProgressBar);
	connect(thread.SongDownloader, &SongDownloader::SetSongCount, this, &PlaylistDownloader::SetSongCount);
	connect(thread.SongDownloader, &SongDownloader::SetSongImage, this, &PlaylistDownloader::SetSongImage);
	connect(thread.SongDownloader, &SongDownloader::SetSongDetails, this, &PlaylistDownloader::SetSongDetails);
	connect(thread.SongDownloader, &SongDownloader::SetErrorItems, this, &PlaylistDownloader::SetErrorItems);
	connect(thread.SongDownloader, &SongDownloader::HidePauseWarning, this, &PlaylistDownloader::HidePauseWarning);
}