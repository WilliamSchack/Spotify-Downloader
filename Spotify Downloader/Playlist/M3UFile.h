#ifndef M3UFILE_H
#define M3UFILE_H

#include "PlaylistFile.h"

class M3UFile : public PlaylistFile {
	public:
		QString Extension() { return "m3u"; }

		QString WriteHeader();
		QString WriteFile(PlaylistFileTrack track);
		QString WriteFooter();
};

#endif