#ifndef XSPFFILE_H
#define XSPFFILE_H

#include "IPlaylistFile.h"
#include "MetadataManager.h"

class XSPFFile : public IPlaylistFile
{
    public:
        std::string GetExtension();
        std::string WriteHeader();
        std::string WriteFile(const PlaylistFileTrack& track);
        std::string WriteFooter();
};

#endif