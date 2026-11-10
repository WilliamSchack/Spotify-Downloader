#ifndef NOTICE_H
#define NOTICE_H

#include <QObject>

class Notice
{
    Q_GADGET
    Q_PROPERTY(int id READ GetId CONSTANT)
    Q_PROPERTY(QString title READ GetTitle CONSTANT)
    Q_PROPERTY(QString releaseDate READ GetReleaseDate CONSTANT)
    Q_PROPERTY(QString content READ GetContent CONSTANT)

    public:
        int     GetId()          const { return Id; }
        QString GetTitle()       const { return Title; }
        QString GetReleaseDate() const { return ReleaseDate; }
        QString GetContent()     const { return Content; }

        int Id = -1;
        QString Title = "";
        QString ReleaseDate = "";
        QString Content = "";
};

#endif