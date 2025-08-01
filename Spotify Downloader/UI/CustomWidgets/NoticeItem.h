#ifndef NOTICEITEM_H
#define NOTICEITEM_H

#include "Network/Notices.h"
#include "Utilities/ObjectHoverWatcher.h"
#include "Utilities/Animation.h"

#include <QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

class NoticeItem : public QPushButton {
	Q_OBJECT

	public:
		explicit NoticeItem(const Notice& notice, ObjectHoverWatcher* objecthoverWatcher = nullptr, QWidget* parent = nullptr);

		void Select();
		void Deselect();
	private:
		inline static const QString TEXT_CONSTRUCTOR = R"(<html><head/><body><p><span style="font-size:10pt;font-weight:700;">%1</span><br/><span style="font-size:8pt;">%2</span></p></body></html>)";

		inline static const QString STYLESHEET = "background-color: rgb(44, 44, 44);border-color: rgb(100, 100, 100);border-style: solid;border-width: 2px;border-radius: 4px;";

		static constexpr QColor DEFAULT_BACKGROUND_COLOUR = QColor(44, 44, 44);
		static constexpr QColor SELECTED_BACKGROUND_COLOUR = QColor(55, 55, 55);

		static constexpr QColor DEFAULT_BORDER_COLOUR = QColor(100, 100, 100);
		static constexpr QColor HOVER_BORDER_COLOUR = QColor(160, 160, 160);
		static constexpr QColor SELECTED_BORDER_COLOUR = QColor(220, 220, 220);

		static constexpr int ANIMATION_TIME_MS = 200;

		QLabel* _textLabel;
	
		QString _title;
		QString _date;

		bool _selected = false;
	private:
		void UpdateText();
};

#endif