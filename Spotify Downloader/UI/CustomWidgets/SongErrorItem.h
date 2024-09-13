#ifndef SONGERRORITEM_H
#define SONGERRORITEM_H

#include <QWidget>
#include <QtWidgets/QLabel>

class SongErrorItem : public QWidget {
	Q_OBJECT

	public:
		SongErrorItem(QWidget* parent = nullptr);
		~SongErrorItem() {}
	
		QLabel* Image;
		QLabel* Title;
		QLabel* Album;
		QLabel* Artists;
};

#endif