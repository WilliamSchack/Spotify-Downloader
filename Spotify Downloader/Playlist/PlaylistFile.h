#ifndef PLAYLISTFILE_H
#define PLAYLISTFILE_H

#include "PlaylistFileTrack.h"

#include "Downloading/Codec.h"
#include "Utilities/StringUtils.h"

#include <QString>

#include <QUrl>
#include <QFile>
#include <QDirIterator>

#include <taglib/fileref.h>

class PlaylistFile {
	public:
		// Creates a playlist file using all the valid songs within the parent folder
		void CreatePlaylistFile(QString parentFolder, QString outputPathWithoutExtension);
	protected:
		virtual QString Extension() = 0;
		virtual QString WriteHeader() = 0;
		virtual QString WriteFile(PlaylistFileTrack track) = 0;
		virtual QString WriteFooter() = 0;
	private:
		inline static QStringList _filtersCache;
	private:
		static QStringList GetFilters();
};

#endif