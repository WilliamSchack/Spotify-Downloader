#ifndef SONG_H
#define SONG_H

#define QT_MESSAGELOGCONTEXT
#include "Utilities/Logger.h"

#include "Downloading/SpotifyDownloader.h"

#include "Network/Network.h"
#include "Network/YTMusicAPI.h"

#include "Utilities/StringUtils.h"
#include "Utilities/MathUtils.h"
#include "Utilities/ImageUtils.h"

#include <QObject>
#include <QProcess>

#include <QJsonArray>
#include <QJsonObject>

#include <QImage>

#include <taglib/fileref.h>
#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/id3v2frame.h>
#include <taglib/commentsframe.h>
#include <taglib/attachedpictureframe.h>
#include <taglib/textidentificationframe.h>

class Song {
	public:
		Song(QJsonObject song, QJsonObject album, QString ytdlpPath, QString ffmpegPath, const QString codec = "mp3", const SpotifyDownloader* main = nullptr);

		void GenerateFileName(const SpotifyDownloader* main);
		void GenerateDownloadingPath();

		void DownloadCoverImage();
		bool SearchForSong(YTMusicAPI*& yt, std::function<void(float)> onProgressUpdate);
		void Download(QProcess*& process, bool overwrite, std::function<void()> onProgressUpdate);
		void SetBitrate(QProcess*& process, int bitrate, std::function<void(float)> onProgressUpdate);
		void NormaliseAudio(QProcess*& process, float normalisedAudioVolume, int bitrate, bool* quitting, std::function<void(float)> onProgressUpdate);
		void AssignMetadata();

		void Save(QString targetFolder, QString targetPath, bool overwrite);

		// --- Song Properties ---
		QString Title;
		QString SpotifyId;
		QString YoutubeId;
		float Time;

		QImage CoverImage;

		// --- Album Properties ---
		QString AlbumName;
		QString AlbumImageURL;
		QJsonArray AlbumArtistsList;
		QStringList AlbumArtistNamesList;
		QString AlbumArtistNames;

		// --- Artist Properties ---
		QString ArtistName;
		QJsonArray ArtistsList;
		QStringList ArtistNamesList;
		QString ArtistNames;

		// --- File Properties ---
		QString FileName;
	private:
		const SpotifyDownloader* _main;

		QJsonArray ScoreSearchResults(QJsonArray searchResults);

		// --- Settings ---
		QString _codec;
		QString _ytdlpPath;
		QString _ffmpegPath;

		// -- Search Result --
		QJsonObject _searchResult;

		// --- Temp Downloading Path ---
		QString _tempPath;
		QString _downloadingFolder;
		QString _downloadingPath;
};

#endif