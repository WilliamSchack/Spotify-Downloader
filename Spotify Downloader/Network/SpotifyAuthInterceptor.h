#ifndef SPOTIFYAUTHINTERCEPTOR_H
#define SPOTIFYAUTHINTERCEPTOR_H

#include <QWebEngineUrlRequestInfo>
#include <QWebEngineUrlRequestInterceptor>
#include <QHash>

class SpotifyAuthInterceptor : public QWebEngineUrlRequestInterceptor
{
    Q_OBJECT

    public:
        QString Authorization = "";
        QString ClientToken = "";
        std::string MobileJsUrl = "";
        bool FoundAll = false;
    public:
        void interceptRequest(QWebEngineUrlRequestInfo &info) override;
    private:
    signals:
        void ValuesRetrieved();
};

#endif
