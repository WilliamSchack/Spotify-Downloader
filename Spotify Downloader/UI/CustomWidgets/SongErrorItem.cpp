#include "SongErrorItem.h"

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

	QFont font3 = QFont();
	font3.setFamily("Segoe UI");
	font3.setPointSize(12);
	font3.setBold(false);
	font1.setWeight(QFont::Weight(50));

	// Setup parent frame
	this->setObjectName("ErrorItem");
	this->setGeometry(QRect(60, 130, 751, 114));
	this->setMinimumSize(0, 114);
	this->setMaximumSize(16777215, 114);

	QWidget* background = new QWidget(this);
	background->setGeometry(QRect(0, 0, 1000, 114));
	background->setStyleSheet("background: rgb(53, 53, 53);");
	background->raise();

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

	// Error Item Artists
	Artists = new QLabel(this);
	Artists->setText("Song Artists");
	Artists->setObjectName("ErrorItemArtists");
	Artists->setGeometry(QRect(90, 60, 661, 20));
	Artists->setFont(font2);
	Artists->setStyleSheet("color: rgb(255, 255, 255);\nbackground-color: rgb(53, 53, 53);");

	// Error Message
	Error = new QLabel(this);
	Error->setText("Error Message");
	Error->setObjectName("ErrorItemErrorMessage");
	Error->setGeometry(QRect(0, 88, 751, 26));
	Error->setFont(font3);
	Error->setStyleSheet("color: #ff6464;\nbackground-color: rgb(40, 40, 40); padding-left: 10px; padding-right: 10px; padding-bottom: 3px;");
}