#ifndef NOTICESSCREENGUI_H
#define NOTICESSCREENGUI_H

#include "Notice.h"

#include <QObject>
#include <QList>
#include <QtQml/qqml.h>

class NoticesScreenGUI : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<Notice> notices READ GetNotices CONSTANT)

    public:
        explicit NoticesScreenGUI(QObject* parent = 0);

        QList<Notice> GetNotices();
    private:
        void UpdateNotices();
    private:
        QList<Notice> _notices;
};

#endif