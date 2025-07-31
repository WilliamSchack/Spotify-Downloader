#ifndef NETWORK_H
#define NETWORK_H

#define QT_MESSAGELOGCONTEXT
#include "Utilities/Logger.h"

#include <QCoreApplication>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include <QtNetwork/QNetworkCookieJar>
#include <QtNetwork/QNetworkCookie>

class Network {
	public:
		static QByteArray Get(QNetworkRequest request, QNetworkCookieJar* cookieJar = nullptr);
		static QByteArray Post(QNetworkRequest request, QByteArray postData, QNetworkCookieJar* cookieJar = nullptr);

		static bool Ping(QUrl serverAddress);

		static QNetworkCookieJar* FromNetscapeCookies(QString netscapeCookies);
};

#endif