#include "NoticeItem.h"

NoticeItem::NoticeItem(const Notice& notice, ObjectHoverWatcher* objecthoverWatcher, QWidget* parent) : QWidget(parent) {
	// Setup parent widget
	this->setObjectName("NoticeItem");
	this->setStyleSheet(IDLE_STYLESHEET);
	this->setMaximumHeight(40); // Dimensions managed by scroll area

	// Add vertical layout
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);

	// Add text
	QFont font = QFont();
	font.setFamily("Segoe UI");
	font.setPointSize(10);

	_textLabel = new QLabel(this);
	_textLabel->setIndent(6);
	_textLabel->setFont(font);

	_title = QString::fromStdString(notice.title);
	_date = QString::fromStdString(notice.date);
	UpdateText();
	
	layout->addWidget(_textLabel);
}

void NoticeItem::UpdateText() {
	QString newText = QString(TEXT_CONSTRUCTOR).arg(_title).arg(_date);
	_textLabel->setText(newText);
}