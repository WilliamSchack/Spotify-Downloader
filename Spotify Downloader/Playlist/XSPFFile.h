#ifndef XSPFFILE_H
#define XSPFFILE_H

#include "PlaylistFile.h"

class XSPFFile : public PlaylistFile {
	public:
		QString Extension() { return "xspf"; }
	
		QString WriteHeader();
		QString WriteFile(PlaylistFileTrack track);
		QString WriteFooter();
};

#endif