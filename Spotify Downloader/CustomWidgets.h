#ifndef CUSTOMWIDGETS_H
#define CUSTOMWIDGETS_H

#include <QWidget>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

class CheckBox : public QPushButton {
	Q_OBJECT

	public:
		CheckBox(QWidget* parent = nullptr);
		~CheckBox() {}

		bool isChecked = false;
	private:
		void setCheckedProperly();
};

class DownloaderThread : public QWidget {
	Q_OBJECT

	public:
		DownloaderThread(QWidget* parent = nullptr);
		~DownloaderThread() {}

		QLabel* SongTitle;
		QLabel* SongArtists;
		QLabel* SongImage;
		QLabel* SongCount;
		QLabel* ProgressLabel;

		QWidget* ProgressBar_Front;
	private:
		QWidget* Background;

		QWidget* ProgressBar_Back;
};

class SongErrorItem : public QWidget {
	Q_OBJECT

	public:
		SongErrorItem(QWidget* parent = nullptr);
		~SongErrorItem() {}

		QLabel* Image;
		QLabel* Title;
		QLabel* Album;
		QLabel* Artists;
	private:
		QWidget* Background;
};

#endif