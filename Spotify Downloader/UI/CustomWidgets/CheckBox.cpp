#include "CheckBox.h"

CheckBox::CheckBox(QWidget* parent) : QPushButton(parent) {
	this->setObjectName("CheckBox");
	this->setFont(QFont("Arial", 16));
	this->setStyleSheet("background-color: rgb(255, 255, 255); color: rgb(0, 0, 0);");
	this->isChecked = false;
	connect(this, &QPushButton::clicked, [=] { setCheckedProperly(); });
}

void CheckBox::setCheckedProperly() {
	if (isChecked) {
		isChecked = false;
		this->setText("");
	}
	else {
		isChecked = true;
		this->setText("✔");
	}
}