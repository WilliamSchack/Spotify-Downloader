#include "CustomWidgets.h"

CheckBox::CheckBox(QWidget* parent) : QPushButton(parent) {
	this->setObjectName("CheckBox");
	//this->setGeometry(QRect(110, 100, 41, 41));
	this->setFont(QFont("Arial", 16));
	this->setStyleSheet("background-color: rgb(255, 255, 255); color: rgb(0, 0, 0);");
	this->isChecked = false;
	connect(this, &QPushButton::clicked, [=] { setCheckedProperly(); });
}

void CheckBox::setCheckedProperly() {
	if (isChecked) {
		isChecked = false;
		this->setText("");
	} else {
		isChecked = true;
		this->setText("✔");
	}
}

DownloaderThread::DownloaderThread(QWidget* parent) :QWidget(parent) {
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

	Background = new QWidget(this);
	Background->setGeometry(QRect(0, 0, 1000, 115));
	Background->setStyleSheet("background: rgb(53, 53, 53);");
	Background->raise();

	SongTitle = new QLabel(this);
	SongTitle->setObjectName("SongTitle");
	SongTitle->setGeometry(QRect(118, 5, 641, 41));
	SongTitle->setFont(font1);
	SongTitle->setStyleSheet("color: rgb(255, 255, 255);\nbackground-color: rgb(53, 53, 53);");
	SongTitle->raise();

	SongImage = new QLabel(this);
	SongImage->setObjectName("SongImage");
	SongImage->setGeometry(QRect(5, 5, 105, 105));
	SongImage->setStyleSheet("background-color: rgb(64, 64, 64);");
	SongImage->setScaledContents(false);
	SongImage->raise();

	SongCount = new QLabel(this);
	SongCount->setObjectName("SongCount");
	SongCount->setGeometry(QRect(8, 78, 41, 30));
	SongCount->setFont(font2);
	SongCount->setStyleSheet(QString::fromUtf8("background-color: rgb(64, 64, 64);\nbackground-color: rgb(54, 54, 54);\ncolor: rgb(255, 255, 255);"));
	SongCount->setMargin(5);
	SongCount->raise();

	ProgressLabel = new QLabel(this);
	ProgressLabel->setObjectName("ProgressLabel");
	ProgressLabel->setGeometry(QRect(555, 76, 201, 31));
	ProgressLabel->setFont(font3);
	ProgressLabel->setStyleSheet("color: rgb(255, 255, 255);\nbackground-color: rgb(53, 53, 53);");
	ProgressLabel->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);
	ProgressLabel->raise();

	ProgressBar_Back = new QWidget(this);
	ProgressBar_Back->setObjectName("ProgressBar_Back");
	ProgressBar_Back->setGeometry(QRect(118, 78, 431, 31));
	ProgressBar_Back->setStyleSheet("background-color: rgb(182, 182, 182);");
	ProgressBar_Back->raise();

	ProgressBar_Front = new QWidget(this);
	ProgressBar_Front->setObjectName("ProgressBar_Front_");
	ProgressBar_Front->setGeometry(QRect(120, 80, 427, 27));
	ProgressBar_Front->setStyleSheet("background-color: rgb(0, 100, 0);");
	ProgressBar_Front->raise();

	SongArtists = new QLabel(this);
	SongArtists->setObjectName("SongArtists_2");
	SongArtists->setGeometry(QRect(118, 42, 641, 31));
	SongArtists->setFont(font3);
	SongArtists->setStyleSheet("color: rgb(255, 255, 255);\nbackground-color: rgb(53, 53, 53);");
	SongArtists->raise();
}

SongErrorItem::SongErrorItem(QWidget* parent) : QWidget(parent) {
	// Fonts
	QFont font1 = QFont();
	font1.setFamily("Segoe UI");
	font1.setPointSize(15);
	font1.setBold(true);
	font1.setWeight(QFont::Weight(75));

	QFont font2 = QFont();
	font2.setFamily("Segoe UI");
	font2.setPointSize(10);
	font2.setBold(false);
	font2.setWeight(QFont::Weight(50));

	// Setup parent frame
	this->setObjectName("ErrorItem");
	this->setGeometry(QRect(60, 130, 751, 91));
	this->setMinimumSize(0, 91);
	this->setMaximumSize(16777215, 91);

	Background = new QWidget(this);
	Background->setGeometry(QRect(0, 0, 1000, 91));
	Background->setStyleSheet("background: rgb(53, 53, 53);");
	Background->raise();

	// Error Item Image
	Image = new QLabel(this);
	Image->setObjectName("ErrorItemImage");
	Image->setGeometry(QRect(10, 10, 71, 71));
	Image->setStyleSheet("background-color: rgb(255, 255, 255);");

	// Error Item Title
	Title = new QLabel(this);
	Title->setObjectName("ErrorItemTitle");
	Title->setText("Song Title");
	Title->setGeometry(QRect(90, 10, 671, 31));
	Title->setFont(font1);
	Title->setAutoFillBackground(false);
	Title->setStyleSheet("color: rgb(255, 255, 255);\nbackground-color: rgb(53, 53, 53);");
	Title->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignTop);

	// Error Item Album
	Album = new QLabel(this);
	Album->setObjectName("ErrorItemAlbum");
	Album->setText("Album Name");
	Album->setGeometry(QRect(90, 40, 661, 20));
	Album->setFont(font2);
	Album->setStyleSheet("color: rgb(255, 255, 255);\nbackground-color: rgb(53, 53, 53);");

	//Error Item Artists
	Artists = new QLabel(this);
	Artists->setText("Song Artists");
	Artists->setObjectName("ErrorItemArtists");
	Artists->setGeometry(QRect(90, 60, 661, 20));
	Artists->setFont(font2);
	Artists->setStyleSheet("color: rgb(255, 255, 255);\nbackground-color: rgb(53, 53, 53);");
}