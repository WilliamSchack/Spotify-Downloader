#ifndef NOTICE_H
#define NOTICE_H

#include "EasyQProperties.h"

#include <QObject>

class Notice : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int index READ GetIndex CONSTANT)
    Q_PROPERTY(QString title READ GetTitle CONSTANT)
    Q_PROPERTY(QString releaseDate READ GetReleaseDate CONSTANT)
    Q_PROPERTY(QString content READ GetContent CONSTANT)
    SIMPLE_QPROPERTY(bool, isNew, IsNew, _isNew)
    
    public:
        explicit Notice(QObject* parent = 0) : QObject(parent) {}

        int     GetIndex()       const { return Index; }
        QString GetTitle()       const { return Title; }
        QString GetReleaseDate() const { return ReleaseDate; }
        QString GetContent()     const { return Content; }

        int     Index = 0;
        QString Title = "";
        QString ReleaseDate = "";
        QString Content = "";
    private:
        bool _isNew = false;
};

#endif