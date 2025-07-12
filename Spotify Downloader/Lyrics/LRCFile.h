#ifndef LRCFILE_H
#define LRCFILE_H

#include "Lyrics.h"

#include "Downloading/Song.h"

class LRCFile {
	public:
		static void CreateLRCFile(const QString& outputPathWithoutExtension, const Song& song, Lyrics& lyrics);
	private:
		static QString CreateHeader(const Song& song);
};

#endif