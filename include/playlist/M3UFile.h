#ifndef M3UFILE_H
#define M3UFILE_H

#include "IPlaylistFile.h"

class M3UFile : public IPlaylistFile
{
    public:
        std::string GetExtension();
		std::string WriteHeader();
		std::string WriteFile(const PlaylistFileTrack& track);
		std::string WriteFooter();
};

#endif