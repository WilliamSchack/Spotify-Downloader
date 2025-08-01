#include "NoticeItem.h"

NoticeItem::NoticeItem(const Notice& notice, ObjectHoverWatcher* objecthoverWatcher, QWidget* parent) : QPushButton(parent) {
	// Setup parent widget
	this->setObjectName("NoticeItem");
	this->setStyleSheet(STYLESHEET);
	this->setMinimumHeight(40);
	this->setMaximumHeight(40);

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

	// Allow hover changes
	if (objecthoverWatcher == nullptr)
		return;

	objecthoverWatcher->AddObjectFunctions(this, [=](QObject* object) {
		if (!_selected) Animation::AnimateStylesheetColour(this, "border-color", HOVER_BORDER_COLOUR, ANIMATION_TIME_MS);
    }, [=](QObject* object) {
		if (!_selected) Animation::AnimateStylesheetColour(this, "border-color", DEFAULT_BORDER_COLOUR, ANIMATION_TIME_MS);
    });
}

void NoticeItem::UpdateText() {
	QString newText = QString(TEXT_CONSTRUCTOR).arg(_title).arg(_date);
	_textLabel->setText(newText);
}

void NoticeItem::Select() {
	if (_selected) return;

	// Update the border and background colours
	Animation::AnimateStylesheetColour(this, "background-color", SELECTED_BACKGROUND_COLOUR, ANIMATION_TIME_MS);
	Animation::AnimateStylesheetColour(this, "border-color", SELECTED_BORDER_COLOUR, ANIMATION_TIME_MS);

	_selected = true;
};

void NoticeItem::Deselect() {
	if (!_selected) return;

	// Update the border and background colours
	Animation::AnimateStylesheetColour(this, "background-color", DEFAULT_BACKGROUND_COLOUR, ANIMATION_TIME_MS);
	Animation::AnimateStylesheetColour(this, "border-color", DEFAULT_BORDER_COLOUR, ANIMATION_TIME_MS);

	_selected = false;
}