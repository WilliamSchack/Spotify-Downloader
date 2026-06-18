#include "SpotifyAuthInterceptor.h"

void SpotifyAuthInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    if (FoundAll)
        return;

    if (info.requestUrl().toString().contains("api-partner.spotify.com")) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        const QHash<QByteArray, QByteArray>& httpHeaders = info.httpHeaders();
        if (httpHeaders.contains("authorization") && Authorization.isEmpty())
            Authorization = httpHeaders["authorization"];
        if (httpHeaders.contains("client-token") && ClientToken.isEmpty())
            ClientToken = httpHeaders["client-token"];
#endif
    }

    // Cant get response body, get the url and do another request for the js
    if (info.requestUrl().toString().contains("open.spotifycdn.com/cdn/build/mobile-web-player/mobile-web-player.") && info.requestUrl().toString().contains("js"))
        MobileJsUrl = info.requestUrl().toString().toStdString();

    if (!Authorization.isEmpty() &&
        !ClientToken.isEmpty() &&
        !MobileJsUrl.empty()) {
        emit ValuesRetrieved();
        FoundAll = true;
    }
}
