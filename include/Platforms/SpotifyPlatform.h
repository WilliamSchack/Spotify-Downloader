#ifndef SPOTIFYPLATFORM_H
#define SPOTIFYPLATFORM_H

#include "IPlatform.h"

#include <iostream>

class SpotifyPlatform : public IPlatform
{
    public:
        bool Download(const std::string& url) override;
};

#endif