#ifndef JSONUTILS_H
#define JSONUTILS_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QBuffer>
#include <QPixmap>

class JSONUtils {
	public:
		static QJsonObject Merge(QJsonObject src, QJsonObject other);
		static QJsonArray Extend(QJsonArray src, QJsonArray other);

		static QJsonValue PixmapToJSON(const QPixmap& p);
		static QPixmap PixmapFromJSON(const QJsonValue& val);
};

#endif