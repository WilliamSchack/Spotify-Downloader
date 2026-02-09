#include "SpotifyAuthRetriever.h"

SpotifyAuth SpotifyAuthRetriever::GetAuth(const std::string& url)
{
    QWebEngineProfile* webProfile = new QWebEngineProfile();
    webProfile->setHttpUserAgent("Mozilla/5.0 (Linux; Android 14) Mobile");
    
    QWebEnginePage* webPage = new QWebEnginePage(webProfile);
    webPage->load(QUrl(QString::fromStdString(url)));

    // intercept query response

    return SpotifyAuth();
}