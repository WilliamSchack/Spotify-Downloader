#ifndef NOTICE_H
#define NOTICE_H

#include <QObject>

class Notice
{
    Q_GADGET
    Q_PROPERTY(int index READ GetIndex CONSTANT)
    Q_PROPERTY(QString title READ GetTitle CONSTANT)
    Q_PROPERTY(QString releaseDate READ GetReleaseDate CONSTANT)
    Q_PROPERTY(QString content READ GetContent CONSTANT)

    public:
        int     GetIndex()       const { return Title; }
        QString GetTitle()       const { return Title; }
        QString GetReleaseDate() const { return ReleaseDate; }
        QString GetContent()     const { return Content; }

        int Index = 0;
        QString Title = "";
        QString ReleaseDate = "";
        QString Content = "";
};

#endif