#include "SpotifyAuthRetriever.h"

SpotifyAuth SpotifyAuthRetriever::GetAuth(const std::string& url)
{
    QWebEngineProfile* webProfile = new QWebEngineProfile();
    webProfile->setHttpUserAgent(QString::fromStdString(USER_AGENT));

    SpotifyAuthInterceptor* interceptor = new SpotifyAuthInterceptor();
    webProfile->setUrlRequestInterceptor(interceptor);

    QWebEnginePage* webPage = new QWebEnginePage(webProfile);
    webPage->load(QUrl(QString::fromStdString(url)));

    // Wait for authorisation
    QEventLoop loop;
    QObject::connect(interceptor, &SpotifyAuthInterceptor::ValuesRetrieved, &loop, &QEventLoop::quit);
    QTimer::singleShot(10000, &loop, &QEventLoop::quit);
    loop.exec();

    webPage->deleteLater();
    webProfile->deleteLater();

    if (!interceptor->FoundAll) {
        std::cout << "Could not retrieve spotify auth" << std::endl;
        return SpotifyAuth();
    }

    // Get mobile js for playlist query sha256Hash, cannot get response body from QWebEnginePage so this is the next best thing
    NetworkRequest mobileJsRequest;
    mobileJsRequest.Url = interceptor->MobileJsUrl;
    mobileJsRequest.SetHeader("User-Agent", USER_AGENT);
    mobileJsRequest.SetHeader("Accept-Encoding", "gzip");

    NetworkResponse mobileJsResponse = mobileJsRequest.Get();
    std::string mobileJs = mobileJsResponse.Body;

    std::smatch matches;
    if (!std::regex_search(mobileJs, matches, std::regex(PLAYLIST_QUERY_REGEX))) {
        std::cout << "Could not retrieve spotify auth" << std::endl;
        return SpotifyAuth();
    }

    SpotifyAuth auth;
    auth.Authorization = interceptor->Authorization;
    auth.ClientToken = interceptor->ClientToken;
    auth.PlaylistQueryHash = matches[1];

    interceptor->deleteLater();

    return auth;
}