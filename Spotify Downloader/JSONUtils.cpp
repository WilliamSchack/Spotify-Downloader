#include "JSONUtils.h"

QJsonObject JSONUtils::Merge(QJsonObject src, QJsonObject other) {
	for (int i = 0; i < other.count(); i++) {
		QString key = other.keys()[i];
		src[key] = other[key];
	}
	return src;
}

QJsonArray JSONUtils::Extend(QJsonArray src, QJsonArray other) {
	foreach(QJsonValue val, other) {
		QJsonObject obj = val.toObject();
		src.append(obj);
	}
	return src;
}

QJsonValue JSONUtils::PixmapToJSON(const QPixmap& p) {
	QBuffer buffer;
	buffer.open(QIODevice::WriteOnly);
	p.save(&buffer, "PNG");
	auto const encoded = buffer.data().toBase64();
	return { QLatin1String(encoded) };
}

QPixmap JSONUtils::PixmapFromJSON(const QJsonValue& val) {
	auto const encoded = val.toString().toLatin1();
	QPixmap p;
	p.loadFromData(QByteArray::fromBase64(encoded), "PNG");
	return p;
}