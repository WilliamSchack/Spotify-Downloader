#ifndef CHECKBOX_H
#define CHECKBOX_H

#include <QtWidgets/QPushButton>

class CheckBox : public QPushButton {
	Q_OBJECT

	public:
		bool isChecked = false;
	public:
		CheckBox(QWidget* parent = nullptr);
		~CheckBox() {}
	private:
		void setCheckedProperly();
};

#endif