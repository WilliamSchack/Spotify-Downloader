#ifndef XSPFFILE_H
#define XSPFFILE_H

#include "IPlaylistFile.h"
#include "MetadataManager.h"

class XSPFFile : public IPlaylistFile
{
    public:
        std::string GetExtension() override;
        std::string WriteHeader() override;
        std::string WriteFile(const PlaylistFileTrack& track) override;
        std::string WriteFooter() override;
};

#endif