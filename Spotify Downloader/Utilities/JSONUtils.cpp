#include "JSONUtils.h"

QJsonValue JSONUtils::Navigate(const QJsonValue& json, const QVariantList& searchKeys) {
	QJsonValue currentValue = json;
	foreach(QVariant key, searchKeys) {
		switch (key.type()) {
			case QVariant::String:
			{
				if (!currentValue.isObject())
					return QJsonValue();

				QJsonObject object = currentValue.toObject();
				QString keyString = key.toString();

				currentValue = object[keyString];
				break;
			}
			case QVariant::Int:
			{
				if (!currentValue.isArray())
					return QJsonValue();

				QJsonArray array = currentValue.toArray();
				int index = key.toInt();

				if (index < 0 || index >= array.size())
					return QJsonValue();

				currentValue = array[index];
				break;
			}
			default:
				return QJsonValue();
		}
	}

	return currentValue;
}

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

bool JSONUtils::BranchExists(QJsonValue json, QStringList target) {
	QJsonValue currentJson = json;
	foreach(QString string, target) {
		// Bit of a strange implementation but it works for my case
		bool stringIsIndex;
		int index = string.toInt(&stringIsIndex);
		if (stringIsIndex && currentJson.isArray()) {
			if (currentJson.toArray().size() >= index) {
				currentJson = currentJson.toArray()[index];
				continue;
			}
		} else {
			QJsonObject currentJsonObject = currentJson.toObject();
			if (currentJsonObject.contains(string)) {
				currentJson = currentJsonObject[string];
				continue;
			}
		}

		return false;
	}

	return true;
}