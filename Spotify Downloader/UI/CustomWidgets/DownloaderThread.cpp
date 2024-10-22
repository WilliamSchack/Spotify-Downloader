#include "DownloaderThread.h"

void DownloaderThread::SetTitle(QString title) {
	_songTitle->setText(title);
}

void DownloaderThread::SetArtists(QString artists) {
	_songArtists->setText(artists);
}

void DownloaderThread::SetImage(QImage image) {
	image = image.scaled(_songImage->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	_songImage->setPixmap(QPixmap::fromImage(image));
}

void DownloaderThread::SetSongCount(int current, int total) {
	_songCount->setText(QString("%1/%2").arg(QString::number(current)).arg(QString::number(total)));
	_songCount->adjustSize();
}

void DownloaderThread::SetProgressText(QString progress) {
	_progressLabel->setText(progress);
}

void DownloaderThread::SetProgressBar(float percentage, int durationMs) {
	// 0 - 1000 for smoother animation
	Animation::AnimateValue(_progressBar, (int)(percentage * 1000), durationMs);
}

CompactDownloaderThread::CompactDownloaderThread(QWidget* parent) : DownloaderThread(parent) {
	QFont font10;
	font10.setPointSize(11);
	font10.setBold(true);

	QFont font11;
	font11.setPointSize(10);
	font11.setBold(true);

	QFont font12;
	font12.setBold(false);

	this->setObjectName("DownloaderThread");
	this->setMinimumSize(QSize(0, 60));
	this->setMaximumSize(QSize(16777215, 60));
	this->setAutoFillBackground(false);
	this->setStyleSheet("background: rgb(53, 53, 53);");

	QGridLayout* gridLayout = new QGridLayout(this);
	gridLayout->setSpacing(0);
	gridLayout->setObjectName("gridLayout");
	gridLayout->setContentsMargins(0, 0, 0, 0);

	QVBoxLayout* VerticalLayout = new QVBoxLayout();
	VerticalLayout->setSpacing(3);
	VerticalLayout->setObjectName("DownloaderThreadVerticalLayout");
	VerticalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
	VerticalLayout->setContentsMargins(-1, 5, 0, 0);

	QVBoxLayout* ContentsVerticalLayout = new QVBoxLayout();
	ContentsVerticalLayout->setSpacing(6);
	ContentsVerticalLayout->setObjectName("DownloaderThreadContentsVerticalLayout");
	ContentsVerticalLayout->setContentsMargins(10, 3, 10, 3);

	_songTitle = new QLabel(this);
	_songTitle->setObjectName("DownloaderThreadTitleLabel");
	_songTitle->setFont(font10);
	_songTitle->setStyleSheet("background-color: transparent;");
	_songTitle->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);

	ContentsVerticalLayout->addWidget(_songTitle);

	QHBoxLayout* ContentsHorizontalLayout = new QHBoxLayout();
	ContentsHorizontalLayout->setObjectName("DownloaderThreadContentsHorizontalLayout");
	ContentsHorizontalLayout->setStretch(0, 300);

	_songArtists = new QLabel(this);
	_songArtists->setObjectName("DownloaderThreadArtistsLabel");
	_songArtists->setFont(font11);
	_songArtists->setStyleSheet(QString::fromUtf8("background-color: transparent;"));

	ContentsHorizontalLayout->addWidget(_songArtists);

	_progressLabel = new QLabel(this);
	_progressLabel->setObjectName("DownloaderThreadProgressText");

	QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	sizePolicy.setHorizontalStretch(100);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(_progressLabel->sizePolicy().hasHeightForWidth());

	_progressLabel->setSizePolicy(sizePolicy);
	_progressLabel->setFont(font12);
	_progressLabel->setStyleSheet(QString::fromUtf8("background-color: transparent;"));
	_progressLabel->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);

	ContentsHorizontalLayout->addWidget(_progressLabel);

	ContentsVerticalLayout->addLayout(ContentsHorizontalLayout);

	VerticalLayout->addLayout(ContentsVerticalLayout);

	_progressBar = new QProgressBar(this);
	_progressBar->setObjectName("DownloaderThreadProgressBar");
	QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Maximum);
	sizePolicy1.setHorizontalStretch(0);
	sizePolicy1.setVerticalStretch(0);
	sizePolicy1.setHeightForWidth(_progressBar->sizePolicy().hasHeightForWidth());
	_progressBar->setSizePolicy(sizePolicy1);
	_progressBar->setMinimumSize(QSize(0, 6));
	_progressBar->setStyleSheet("QProgressBar { width: 1px; } QProgressBar::chunk { background-color: #3add36; }");
	_progressBar->setValue(50);
	_progressBar->setTextVisible(false);
	_progressBar->setMaximum(1000);

	VerticalLayout->addWidget(_progressBar);

	gridLayout->addLayout(VerticalLayout, 0, 0, 1, 1);
}

void CompactDownloaderThread::paintEvent(QPaintEvent* e)
{
	QPainter painter(this);

	if (_currentImage.isNull()) {
		QStyleOption opt;
		opt.initFrom(this);
		style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
	}
	else {
		painter.drawImage(this->rect(), _currentImage);
	}

	QWidget::paintEvent(e);
}

void CompactDownloaderThread::SetSongCount(int current, int total) {
	_currentSongCountText = QString("%1/%2").arg(QString::number(current)).arg(QString::number(total));

	_progressLabel->setText(QString("%1 <b>%2</b>").arg(_currentProgressText).arg(_currentSongCountText));
}

void CompactDownloaderThread::SetProgressText(QString progress) {
	_currentProgressText = progress;

	_progressLabel->setText(QString("%1 <b>%2</b>").arg(_currentProgressText).arg(_currentSongCountText));
}

void CompactDownloaderThread::SetImage(QImage image) {
	// Get image crop rect
	QRect imageRect;
	imageRect.setX(0);
	imageRect.setY(image.height() / 2); // Center cover art
	imageRect.setSize(this->size());

	// Scale to widget size
	image = image.scaledToWidth(this->width());
	image = image.copy(imageRect);

	// Apply blur and darken image
	QGraphicsBlurEffect* blur = new QGraphicsBlurEffect();
	blur->setBlurRadius(6);

	image = ImageUtils::ApplyEffectToImage(image, blur);
	image = ImageUtils::AdjustBrightness(image, 0.6);

	// Copy image cropped to widget
	_currentImage = image;
	this->update();
}

OriginalDownloaderThread::OriginalDownloaderThread(QWidget* parent) : DownloaderThread(parent) {
	QFont font1 = QFont();
	font1.setFamily("Segoe UI");
	font1.setPointSize(20);
	font1.setBold(true);

	QFont font2 = QFont();
	font2.setFamily("Impact");
	font2.setPointSize(12);
	font2.setBold(false);

	QFont font3 = QFont();
	font3.setFamily("Segoe UI");
	font3.setPointSize(12);
	font3.setBold(true);

	this->setObjectName("DownloaderThread");
	this->setGeometry(QRect(14, 15, 759, 115));
	this->setMinimumSize(0, 115);
	this->setMaximumSize(16777215, 115);

	QWidget* background = new QWidget(this);
	background->setGeometry(QRect(0, 0, 1000, 115));
	background->setStyleSheet("background: rgb(53, 53, 53);");
	background->raise();

	_songTitle = new QLabel(this);
	_songTitle->setObjectName("SongTitle");
	_songTitle->setGeometry(QRect(118, 5, 587, 41));
	_songTitle->setFont(font1);
	_songTitle->setStyleSheet("color: rgb(255, 255, 255);\nbackground-color: rgb(53, 53, 53);");
	_songTitle->raise();

	_songImage = new QLabel(this);
	_songImage->setObjectName("SongImage");
	_songImage->setGeometry(QRect(5, 5, 105, 105));
	_songImage->setStyleSheet("background-color: rgb(64, 64, 64);");
	_songImage->setScaledContents(false);
	_songImage->raise();

	_songCount = new QLabel(this);
	_songCount->setObjectName("SongCount");
	_songCount->setGeometry(QRect(8, 78, 41, 30));
	_songCount->setFont(font2);
	_songCount->setStyleSheet(QString::fromUtf8("background-color: rgb(64, 64, 64);\nbackground-color: rgb(54, 54, 54);\ncolor: rgb(255, 255, 255);"));
	_songCount->setMargin(5);
	_songCount->raise();

	_progressLabel = new QLabel(this);
	_progressLabel->setObjectName("ProgressLabel");
	_progressLabel->setGeometry(QRect(501, 76, 201, 31));
	_progressLabel->setFont(font3);
	_progressLabel->setStyleSheet("color: rgb(255, 255, 255);\nbackground-color: rgb(53, 53, 53);");
	_progressLabel->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);
	_progressLabel->raise();

	QWidget* progressBarBack = new QWidget(this);
	progressBarBack->setObjectName("ProgressBar_Back");
	progressBarBack->setGeometry(QRect(118, 78, 377, 31));
	progressBarBack->setStyleSheet("background-color: rgb(182, 182, 182);");
	progressBarBack->raise();

	_progressBar = new QProgressBar(this);
	_progressBar->setObjectName("ProgressBar");
	_progressBar->setGeometry(QRect(120, 80, 373, 27));
	_progressBar->setStyleSheet("QProgressBar { width: 1px; } QProgressBar::chunk { background-color: rgb(0, 150, 0); }");
	_progressBar->setTextVisible(false);
	_progressBar->setMaximum(1000);
	_progressBar->raise();

	_songArtists = new QLabel(this);
	_songArtists->setObjectName("SongArtists_2");
	_songArtists->setGeometry(QRect(118, 42, 587, 31));
	_songArtists->setFont(font3);
	_songArtists->setStyleSheet("color: rgb(255, 255, 255);\nbackground-color: rgb(53, 53, 53);");
	_songArtists->raise();
}