#include "SpotifyAuthRetriever.h"

SpotifyAuth SpotifyAuthRetriever::GetAuth(const QUrl& url)
{
    QWebEngineProfile* webProfile = new QWebEngineProfile();
    webProfile->setHttpUserAgent(USER_AGENT);

    SpotifyAuthInterceptor* interceptor = new SpotifyAuthInterceptor();
    webProfile->setUrlRequestInterceptor(interceptor);

    QWebEnginePage* webPage = new QWebEnginePage(webProfile);
    webPage->load(url);

    // Wait for authorisation
    QEventLoop loop;
    QObject::connect(interceptor, &SpotifyAuthInterceptor::ValuesRetrieved, &loop, &QEventLoop::quit);
    QTimer::singleShot(TIMEOUT_MS, &loop, &QEventLoop::quit);
    loop.exec();

    SpotifyAuth auth;
    bool foundAll = interceptor->FoundAll;
    QString mobileJsUrl = "";

    if (foundAll) {
        auth.Authorization = interceptor->Authorization.toUtf8();
        auth.ClientToken = interceptor->ClientToken.toUtf8();
        mobileJsUrl = QString::fromStdString(interceptor->MobileJsUrl);
    }

    delete webPage;
    delete interceptor;
    delete webProfile;

    if (!foundAll) {
        qWarning() << "Could not retrieve spotify auth";
        return auth;
    }

    // Get mobile js for playlist query sha256Hash, cannot get response body from QWebEnginePage so this is the next best thing
    QNetworkRequest mobileJsRequest = QNetworkRequest(mobileJsUrl);
    mobileJsRequest.setRawHeader("User-Agent", USER_AGENT);
    QByteArray mobileJs = Network::Get(mobileJsRequest);

    QRegularExpression regex(PLAYLIST_QUERY_REGEX);
    QStringList matches = regex.match(mobileJs).capturedTexts();

    if (matches.size() < 2) {
        qWarning() << "Could not retrieve spotify auth";
        return SpotifyAuth();
    }

    auth.PlaylistQueryHash = matches[1];

    return auth;
}