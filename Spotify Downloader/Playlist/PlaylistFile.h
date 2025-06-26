#ifndef PLAYLISTFILE_H
#define PLAYLISTFILE_H

#include "Downloading/Codec.h"

#include <QString>

#include <QFile>
#include <QDirIterator>

class PlaylistFile {
	public:
		// Creates a playlist file using all the valid songs within the parent folder
		void CreatePlaylistFile(QString parentFolder, QString outputPathWithoutExtension);
	protected:
		virtual QString Extension() = 0;
		virtual QString WriteHeader() = 0;
		virtual QString WriteFile(QFileInfo fileInfo) = 0;
		virtual QString WriteFooter() = 0;
	private:
		inline static QStringList _filtersCache;
	private:
		static QStringList GetFilters();
};

#endif