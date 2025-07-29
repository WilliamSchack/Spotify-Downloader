#ifndef SONGERRORITEM_H
#define SONGERRORITEM_H

#include <QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLineEdit>

#include <QDesktopServices>

class SongErrorItem : public QWidget {
	Q_OBJECT

	public:
		QLabel* Image;
		QLabel* Title;
		QLabel* Album;
		QLabel* Artists;
		QLabel* Error;
	public:
		SongErrorItem(QWidget* parent = nullptr);
		~SongErrorItem() {}

		void AddLinkInput(QString searchQuery);
};

#endif