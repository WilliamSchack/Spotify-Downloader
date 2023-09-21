#pragma once

#include <QWidget>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

class SongErrorItem : public QWidget {
	Q_OBJECT

	public:
		SongErrorItem(QWidget *parent = nullptr);
		~SongErrorItem() {}

		QLabel* Image;
		QLabel* Title;
		QLabel* Album;
		QLabel* Artists;
};

class CheckBox : public QPushButton {
	Q_OBJECT

	public:
		CheckBox(QWidget* parent = nullptr);
		~CheckBox() {}

		bool isChecked = false;
	private:
		void setCheckedProperly();
};