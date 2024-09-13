#ifndef DOWNLOADERTHREAD_H
#define DOWNLOADERTHREAD_H

#include "Utilities/Animation.h"
#include "Utilities/ImageUtils.h"

#include <QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QGridLayout>

#include <QPainter>
#include <QGraphicsBlurEffect>
#include <QStyleOption>

class DownloaderThread : public QWidget {
	Q_OBJECT

	public:
		DownloaderThread(QWidget* parent = nullptr) { }
		~DownloaderThread() {}
	
		virtual void SetTitle(QString title);
		virtual void SetArtists(QString artists);
		virtual void SetImage(QImage image);
		virtual void SetSongCount(int current, int total);
		virtual void SetProgressText(QString progress);
		virtual void SetProgressBar(float percentage, int durationMs = 500);
	protected:
		QLabel* _songTitle;
		QLabel* _songArtists;
		QLabel* _songImage;
		QLabel* _songCount;
		QLabel* _progressLabel;
		QProgressBar* _progressBar;
};

class CompactDownloaderThread : public DownloaderThread {
	public:
		CompactDownloaderThread(QWidget* parent = nullptr);
	
		void paintEvent(QPaintEvent*) override;
	
		void SetSongCount(int current, int total) override;
		void SetProgressText(QString progress) override;
		void SetImage(QImage image) override;
	private:
		QString _currentSongCountText = "0/0";
		QString _currentProgressText = "";
		QImage _currentImage = QImage();
	};

class OriginalDownloaderThread : public DownloaderThread {
	public:
		OriginalDownloaderThread(QWidget* parent = nullptr);
};

#endif