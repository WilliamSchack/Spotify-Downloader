#ifndef NETWORK_H
#define NETWORK_H

#define QT_MESSAGELOGCONTEXT
#include "Logger.h"

#include <QCoreApplication>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

class Network {
	public:
		static QByteArray Get(QNetworkRequest request);
		static QByteArray Post(QNetworkRequest request, QByteArray postData);

		static bool Ping(QUrl serverAddress);
};

#endif