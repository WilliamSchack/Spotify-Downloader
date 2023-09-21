#include "CustomWidgets.h"

SongErrorItem::SongErrorItem(QWidget* parent) : QWidget(parent) {
	// Fonts
	QFont font = QFont();
	font.setFamily("Segoe UI");
	font.setPointSize(15);
	font.setBold(true);
	font.setWeight(QFont::Weight(75));

	QFont font1 = QFont();
	font1.setFamily("Segoe UI");
	font1.setPointSize(10);
	font1.setBold(false);
	font1.setWeight(QFont::Weight(50));

	// Setup parent frame
	this->setObjectName("ErrorItem");
	this->setGeometry(QRect(60, 130, 751, 91));
	this->setMinimumSize(0, 91);
	this->setMaximumSize(16777215, 91);
	this->setStyleSheet("background-color: rgb(53, 53, 53);");

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
	Title->setFont(font);
	Title->setAutoFillBackground(false);
	Title->setStyleSheet("color: rgb(255, 255, 255);\nbackground-color: rgb(53, 53, 53);");
	Title->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignTop);

	// Error Item Album
	Album = new QLabel(this);
	Album->setObjectName("ErrorItemAlbum");
	Album->setText("Album Name");
	Album->setGeometry(QRect(90, 40, 661, 20));
	Album->setFont(font1);
	Album->setStyleSheet("color: rgb(255, 255, 255);\nbackground-color: rgb(53, 53, 53);");

	//Error Item Artists
	Artists = new QLabel(this);
	Artists->setText("Song Artists");
	Artists->setObjectName("ErrorItemArtists");
	Artists->setGeometry(QRect(90, 60, 661, 20));
	Artists->setFont(font1);
	Artists->setStyleSheet("color: rgb(255, 255, 255);\nbackground-color: rgb(53, 53, 53);");
}

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