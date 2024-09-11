#include "ImageUtils.h"

void ImageUtils::DownloadImage(QString url, QString path, QSize resize) {
	QNetworkRequest req(url);
	QByteArray response = Network::Get(req);
	QImage image;
	image.loadFromData(response);
	if (!resize.isEmpty()) image = image.scaled(resize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	image.save(path);
}

QImage ImageUtils::ApplyEffectToImage(QImage src, QGraphicsEffect* effect, int extent) {
    if (src.isNull()) return QImage();
    if (!effect) return src;

    QGraphicsScene scene;
    QGraphicsPixmapItem item;
    item.setPixmap(QPixmap::fromImage(src));
    item.setGraphicsEffect(effect);
    scene.addItem(&item);
    QImage res(src.size() + QSize(extent * 2, extent * 2), QImage::Format_ARGB32);
    res.fill(Qt::transparent);
    QPainter ptr(&res);
    scene.render(&ptr, QRectF(), QRectF(-extent, -extent, src.width() + extent * 2, src.height() + extent * 2));
    return res;
}

QImage ImageUtils::AdjustBrightness(QImage src, float factor) {
    for (int x = 0; x < src.width(); x++) {
        for (int y = 0; y < src.height(); y++) {
            QColor color(src.pixel(x, y));
            if (color.alpha() != 0) {
                color.setHsv(color.hue(), color.saturation(), color.value() * factor, color.alpha());
                //color.setRgb(color.red() * factor, color.blue() * factor, color.green() * factor, color.alpha());
                src.setPixelColor(x, y, color);
            }
        }
    }

    return src;
}