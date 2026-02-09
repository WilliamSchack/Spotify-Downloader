#ifndef SPOTIFYAUTHRETRIEVER_H
#define SPOTIFYAUTHRETRIEVER_H

#include "SpotifyAuth.h"

#include <QUrl>
#include <QtWebEngineCore>

class SpotifyAuthRetriever
{
    public:
        static SpotifyAuth GetAuth(const std::string& url);
};

#endif