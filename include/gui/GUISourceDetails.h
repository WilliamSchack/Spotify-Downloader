#ifndef GUISOURCEDETAILS_H
#define GUISOURCEDETAILS_H

#include <QObject>

class GUISourceDetails
{
    Q_GADGET
    Q_PROPERTY(QString iconSource READ GetIconSource CONSTANT)
    Q_PROPERTY(QString body READ GetBody CONSTANT)

    public:
        QString GetIconSource() const { return IconSource; }
        QString GetLink()       const { return Link; }

        QString IconSource = "";
        QString Link = "";
};

#endif