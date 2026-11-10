#include "NoticesScreenGUI.h"

NoticesScreenGUI::NoticesScreenGUI(QObject* parent) : QObject(parent)
{
    qmlRegisterUncreatableType<Notice>("Echolite.Info.Notices", 1, 0, "Notice", "Created in c++");

    UpdateNotices();
}

QList<Notice> NoticesScreenGUI::GetNotices()
{
    return _notices;
}

void NoticesScreenGUI::UpdateNotices()
{
    
}