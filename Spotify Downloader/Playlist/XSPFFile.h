#ifndef XSPFFILE_H
#define XSPFFILE_H

#include "PlaylistFile.h"
#include "Utilities/StringUtils.h";

#include <QUrl>

#include <taglib/fileref.h>

class XSPFFile : public PlaylistFile {
	public:
		QString Extension() { return "xspf"; }
	
		QString WriteHeader();
		QString WriteFile(QFileInfo fileInfo);
		QString WriteFooter();
};

#endif