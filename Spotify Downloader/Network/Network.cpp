#include "Network.h"

QByteArray Network::Get(QNetworkRequest request, QNetworkCookieJar* cookieJar) {
	QNetworkAccessManager* manager = new QNetworkAccessManager();
	if (cookieJar != nullptr)
		manager->setCookieJar(cookieJar);

	bool complete = false;
	QByteArray response;
	QNetworkReply* rep = manager->get(request);
	QObject::connect(manager, &QNetworkAccessManager::finished, [&complete, &response](QNetworkReply* reply) {
		QVariantList sentCookies = reply->request().header(QNetworkRequest::CookieHeader).toList();
		if (reply->error() != QNetworkReply::NoError) {
			qWarning() << reply->errorString();

			response = nullptr;
			complete = true;
			return;
		}

		response = reply->readAll();
		complete = true;
	});

	QObject::connect(manager, &QNetworkAccessManager::finished, manager, &QNetworkAccessManager::deleteLater);
	QObject::connect(manager, &QNetworkAccessManager::finished, rep, &QNetworkReply::deleteLater);

	while (!complete)
		QCoreApplication::processEvents();

	return response;
}

QByteArray Network::Post(QNetworkRequest request, QByteArray postData, QNetworkCookieJar* cookieJar) {
	QNetworkAccessManager* manager = new QNetworkAccessManager();
	if (cookieJar != nullptr)
		manager->setCookieJar(cookieJar);

	bool complete = false;
	QByteArray response;
	QNetworkReply* rep = manager->post(request, postData);
	QObject::connect(manager, &QNetworkAccessManager::finished, [&complete, &response](QNetworkReply* reply) {
		if (reply->error() != QNetworkReply::NoError) {
			qWarning() << reply->errorString();

			response = nullptr;
			complete = true;
			return;
		}

		response = reply->readAll();
		complete = true;
	});

	QObject::connect(manager, &QNetworkAccessManager::finished, manager, &QNetworkAccessManager::deleteLater);
	QObject::connect(manager, &QNetworkAccessManager::finished, rep, &QNetworkReply::deleteLater);

	while (!complete)
		QCoreApplication::processEvents();

	return response;
}

bool Network::Ping(QUrl serverAddress) {
	QNetworkAccessManager* manager = new QNetworkAccessManager();
	QNetworkRequest request(serverAddress);

	bool complete = false;
	bool serverResponded = false;
	QNetworkReply* rep = manager->get(request);
	QObject::connect(manager, &QNetworkAccessManager::finished, [&complete, &serverResponded](QNetworkReply* reply) {
		serverResponded = reply->bytesAvailable();
		complete = true;
	});

	QObject::connect(manager, &QNetworkAccessManager::finished, manager, &QNetworkAccessManager::deleteLater);
	QObject::connect(manager, &QNetworkAccessManager::finished, rep, &QNetworkReply::deleteLater);

	while (!complete)
		QCoreApplication::processEvents();

	return serverResponded;
}

QNetworkCookieJar* Network::FromNetscapeCookies(QString netscapeCookies) {
	QNetworkCookieJar* cookieJar = new QNetworkCookieJar();

	QStringList cookieLines = netscapeCookies.split("\n");
	for (int i = 0; i < cookieLines.size(); i++) {
		QString line = cookieLines[i];

		if (line.isEmpty())
			continue;

		if (line.startsWith("#HttpOnly_"))
			line.replace("#HttpOnly_", "");

		if (line.startsWith("#"))
			continue;

		QStringList fields = line.split("\t");
		if (fields.size() != 7)
			continue;

		QString host = fields[0];
		QString path = fields[2];
		bool secure = fields[3] == "TRUE";
		qint64 expiry = fields[4].toLongLong();
		QString name = fields[5];
		QString value = fields[6];

		QNetworkCookie cookie(name.toUtf8(), value.toUtf8());
		cookie.setDomain(host);
		cookie.setPath(path);
		cookie.setSecure(secure);
		cookie.setExpirationDate(QDateTime::fromSecsSinceEpoch(expiry));

		cookieJar->insertCookie(cookie);
	}

	return cookieJar;
}