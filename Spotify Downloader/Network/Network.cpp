#include "Network.h"

QByteArray Network::Get(QNetworkRequest request) {
	QNetworkAccessManager* manager = new QNetworkAccessManager();

	bool complete = false;
	QByteArray response;
	QNetworkReply* rep = manager->get(request);
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

QByteArray Network::Post(QNetworkRequest request, QByteArray postData) {
	QNetworkAccessManager* manager = new QNetworkAccessManager();

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