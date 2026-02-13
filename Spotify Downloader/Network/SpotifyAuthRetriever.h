#ifndef SPOTIFYAUTHRETRIEVER_H
#define SPOTIFYAUTHRETRIEVER_H

#include "SpotifyAuth.h"
#include "SpotifyAuthInterceptor.h"
#include "Network/Network.h"

#include <QUrl>
#include <QEventLoop>
#include <QtWebEngineCore>

#include <iostream>
#include <regex>

class SpotifyAuthRetriever
{
    public:
        static SpotifyAuth GetAuth(const std::string& url);
    private:
        static inline const QByteArray USER_AGENT = "Mozilla/5.0 (Linux; Android 14) Mobile";
        static inline const QString PLAYLIST_QUERY_REGEX = "\"queryPlaylist\",\"query\",\"(.*?)\"";
        static inline const unsigned int TIMEOUT_MS = 10000;
};

#endif