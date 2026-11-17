#ifndef SPOTIFYAUTH_H
#define SPOTIFYAUTH_H

#include <string>

struct SpotifyAuth
{
    std::string Authorization = "";
    std::string ClientToken = "";
    std::string PlaylistQueryHash = "";
};

#endif