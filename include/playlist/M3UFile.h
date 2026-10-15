#ifndef M3UFILE_H
#define M3UFILE_H

#include "IPlaylistFile.h"

class M3UFile : public IPlaylistFile
{
    public:
        std::string GetExtension() override;
        std::string WriteHeader() override;
        std::string WriteFile(const PlaylistFileTrack& track) override;
        std::string WriteFooter() override;
};

#endif