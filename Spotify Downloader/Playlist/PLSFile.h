#ifndef PLSFILE_H
#define PLSFILE_H

#include "PlaylistFile.h"

class PLSFile : public PlaylistFile {
	public:
		QString Extension() { return "pls"; }

		QString WriteHeader();
		QString WriteFile(PlaylistFileTrack track);
		QString WriteFooter();
	private:
		int _currentFile = 1;
};

#endif