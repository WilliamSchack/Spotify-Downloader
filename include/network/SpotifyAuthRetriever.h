#ifndef SPOTIFYAUTHRETRIEVER_H
#define SPOTIFYAUTHRETRIEVER_H

#include "SpotifyAuth.h"
#include "SpotifyAuthInterceptor.h"
#include "NetworkRequest.h"

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
        static inline const std::string USER_AGENT = "Mozilla/5.0 (Linux; Android 14) Mobile";
        static inline const std::string PLAYLIST_QUERY_REGEX = "\"queryPlaylist\",\"query\",\"(.*?)\"";
};

#endif