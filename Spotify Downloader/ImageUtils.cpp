#include "ImageUtils.h"

void ImageUtils::DownloadImage(QString url, QString path, QSize resize) {
	QNetworkRequest req(url);
	QByteArray response = Network::Get(req);
	QImage image;
	image.loadFromData(response);
	if (!resize.isEmpty()) image = image.scaled(resize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	image.save(path);
}