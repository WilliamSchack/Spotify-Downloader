#ifndef IMAGEUTILS_H
#define IMAGEUTILS_H

#define QT_MESSAGELOGCONTEXT
#include "Logger.h"

#include "Network.h"

#include <QImage>
#include <QString>
#include <QSize>
#include <QNetworkRequest>

class ImageUtils {
	public:
		static void DownloadImage(QString url, QString path, QSize resize);
};

#endif