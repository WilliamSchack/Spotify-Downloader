#ifndef SONG_H
#define SONG_H

#define QT_MESSAGELOGCONTEXT
#include "Utilities/Logger.h"

#include "SpotifyDownloader.h"

#include "Network/Network.h"
#include "Network/MusixmatchAPI.h"
#include "Network/YTMusicAPI.h"

#include "Utilities/StringUtils.h"
#include "Utilities/MathUtils.h"
#include "Utilities/ImageUtils.h"
#include "Utilities/FileUtils.h"

#include "Codec.h"

#include <QObject>
#include <QProcess>

#include <QJsonArray>
#include <QJsonObject>

#include <QImage>

#include <QDate>

#include <taglib/fileref.h>
#include <taglib/tstringlist.h>

#include <taglib/id3v2tag.h>
#include <taglib/id3v2frame.h>
#include <taglib/commentsframe.h>
#include <taglib/attachedpictureframe.h>
#include <taglib/textidentificationframe.h>
#include <taglib/unsynchronizedlyricsframe.h>
#include <taglib/synchronizedlyricsframe.h>

#include <taglib/mp4file.h>
#include <taglib/mp4tag.h>
#include <taglib/mp4coverart.h>

#include <taglib/wavfile.h>

#include <taglib/flacpicture.h>
#include <taglib/xiphcomment.h>

class Song {
	public:
		// --- Song Properties ---
		QString Title = "";
		QString SpotifyId = "";
		QString YoutubeId = "";
		QString Isrc = "";
		float Time = 0;
		bool IsExplicit;
		QDate ReleaseDate;

		int PlaylistTrackNumber = 0;
		int AlbumTrackNumber = 0;
		int DiscNumber = 0;

		MusixmatchAPI::LyricsType LyricsType = MusixmatchAPI::LyricsType::None;
		QString Lyrics = "";
		QList<MusixmatchAPI::SynchronisedLyric> SyncedLyrics;

		bool InPlaylist;

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
		Codec::Extension Codec;
	public:
		Song(QJsonObject song, QJsonObject album, QString ytdlpPath, QString ffmpegPath, Codec::Extension codec, const SpotifyDownloader* main = nullptr);

		// Helpers for config naming tags
		static std::tuple<QString, bool> TagHandler(Song song, QString tag);
		static std::tuple<QString, Config::NamingError> OutputNameWithTags(Song song);
		static std::tuple<QString, Config::NamingError> SubFoldersWithTags(Song song);

		void GenerateFileName(const SpotifyDownloader* main);
		void GenerateDownloadingPath();

		void DownloadCoverImage();
		QString SearchForSong(YTMusicAPI*& yt, std::function<void(float)> onProgressUpdate);
		QString Download(YTMusicAPI*& yt, QProcess*& process, bool overwrite, std::function<void(float)> onProgressUpdate, std::function<void()> onPOTokenWarning = nullptr, std::function<void()> onLowQualityWarning = nullptr, std::function<void()> onPremiumDisabled = nullptr);
		void SetBitrate(QProcess*& process, int bitrate, std::function<void(float)> onProgressUpdate);
		void NormaliseAudio(QProcess*& process, float normalisedAudioVolume, int bitrate, bool* quitting, std::function<void(float)> onProgressUpdate);
		void GetLyrics();
		void AssignMetadata();

		void Save(QString targetFolder, QString targetPath, bool overwrite);
	private:
		const SpotifyDownloader* _main;

		// --- Settings ---
		QString _ytdlpPath;
		QString _ffmpegPath;

		// -- Search Result --
		QJsonObject _searchResult;

		// --- Temp Downloading Path ---
		QString _tempPath;
		QString _downloadingFolder;
		QString _downloadingPath;
	private:
		int TrackNumber();

		QJsonArray ScoreSearchResults(QJsonArray searchResults);
};

#endif