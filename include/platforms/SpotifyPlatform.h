#ifndef SPOTIFYPLATFORM_H
#define SPOTIFYPLATFORM_H

#include "IPlatform.h"

class SpotifyPlatform : public IPlatform
{
    private:
        ELinkType GetLinkType(const std::string& url) override;
};

#endif