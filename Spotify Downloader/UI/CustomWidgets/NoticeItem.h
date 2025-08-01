#ifndef NOTICEITEM_H
#define NOTICEITEM_H

#include "Network/Notices.h"
#include "Utilities/ObjectHoverWatcher.h"
//#include "Utilities/Animation.h"

#include <QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

class NoticeItem : public QWidget {
	public:
		NoticeItem(const Notice& notice, ObjectHoverWatcher* objecthoverWatcher = nullptr, QWidget* parent = nullptr);
		~NoticeItem() {}
	private:
		static inline const QString IDLE_STYLESHEET = "background-color: rgb(44, 44, 44);border-color: rgb(100, 100, 100);border-style: solid;border-width: 2px;border-radius: 4px;";
		static inline const QString SELECTED_STYLESHEET = "background-color: rgb(55, 55, 55);border-color: rgb(220, 220, 220);border-style: solid;border-width: 2px;border-radius: 4px;";
		static inline const QString UNREAD_STYLESHEET = "background-color: rgb(44, 44, 44);border-color: rgb(100, 100, 100);border-bottom-color: red;border-style: solid;border-width: 2px;border-radius: 4px;";
		
		static inline const QString TEXT_CONSTRUCTOR = R"(<html><head/><body><p><span style="font-size:10pt;font-weight:700;">%1</span><br/><span style="font-size:8pt;">%2</span></p></body></html>)";
	
		QLabel* _textLabel;
	
		QString _title;
		QString _date;
	private:
		void UpdateText();
};

#endif