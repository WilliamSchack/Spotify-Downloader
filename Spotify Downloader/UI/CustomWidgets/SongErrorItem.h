#ifndef SONGERRORITEM_H
#define SONGERRORITEM_H

#include <QWidget>
#include <QtWidgets/QLabel>

class SongErrorItem : public QWidget {
	Q_OBJECT

	public:
		QLabel* Image;
		QLabel* Title;
		QLabel* Album;
		QLabel* Artists;
	public:
		SongErrorItem(QWidget* parent = nullptr);
		~SongErrorItem() {}
};

#endif