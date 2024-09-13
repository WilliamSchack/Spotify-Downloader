#ifndef CHECKBOX_H
#define CHECKBOX_H

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

#endif