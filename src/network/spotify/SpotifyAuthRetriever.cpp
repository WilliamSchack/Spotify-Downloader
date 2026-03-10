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
    QTimer::singleShot(TIMEOUT_MS, &loop, &QEventLoop::quit);
    loop.exec();

    SpotifyAuth auth;
    bool foundAll = interceptor->FoundAll;
    std::string mobileJsUrl = "";

    if (foundAll) {
        auth.Authorization = interceptor->Authorization;
        auth.ClientToken = interceptor->ClientToken;
        mobileJsUrl = interceptor->MobileJsUrl;
    }

    delete webPage;
    delete interceptor;
    delete webProfile;

    if (!foundAll) {
        std::cout << "Could not retrieve spotify auth" << std::endl;
        return SpotifyAuth();
    }

    // Get mobile js for playlist query sha256Hash, cannot get response body from QWebEnginePage so this is the next best thing
    NetworkRequest mobileJsRequest;
    mobileJsRequest.Url = mobileJsUrl;
    mobileJsRequest.SetHeader("User-Agent", USER_AGENT);
    mobileJsRequest.SetHeader("Accept-Encoding", "gzip");

    NetworkResponse mobileJsResponse = mobileJsRequest.Get();
    std::string mobileJs = mobileJsResponse.Body;

    std::smatch matches;
    if (!std::regex_search(mobileJs, matches, std::regex(PLAYLIST_QUERY_REGEX))) {
        std::cout << "Could not retrieve spotify auth" << std::endl;
        return SpotifyAuth();
    }
    auth.PlaylistQueryHash = matches[1];

    return auth;
}