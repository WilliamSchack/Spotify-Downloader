#ifndef IMAGEUTILS_H
#define IMAGEUTILS_H

#define QT_MESSAGELOGCONTEXT
#include "Logger.h"

#include "Network.h"

#include <QImage>
#include <QString>
#include <QSize>
#include <QNetworkRequest>

#include <QPainter>
#include <QGraphicsEffect>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

class ImageUtils {
	public:
		static void DownloadImage(QString url, QString path, QSize resize);

		static QImage ApplyEffectToImage(QImage src, QGraphicsEffect* effect, int extent = 0);
		static QImage AdjustBrightness(QImage src, float factor); // 0-1 for darken, 1+ for lighten
};

#endif