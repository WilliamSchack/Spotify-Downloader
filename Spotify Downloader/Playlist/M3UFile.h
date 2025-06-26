#ifndef M3UFILE_H
#define M3UFILE_H

#include "PlaylistFile.h"
#include "Utilities/StringUtils.h";

#include <QUrl>

#include <taglib/fileref.h>

class M3UFile : public PlaylistFile {
	public:
		QString Extension() { return "m3u"; }

		QString WriteHeader();
		QString WriteFile(QFileInfo fileInfo);
		QString WriteFooter();
};

#endif