#ifndef SPOTIFYPLATFORM_H
#define SPOTIFYPLATFORM_H

#include <iostream>

#include "IPlatform.h"

class SpotifyPlatform : public IPlatform
{
    public:
        bool Download(const std::string& url) override;
};

#endif