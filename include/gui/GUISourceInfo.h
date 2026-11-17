#ifndef GUISOURCEINFO_H
#define GUISOURCEINFO_H

#include "EPlatform.h"

#include <QObject>

class GUISourceInfo
{
    Q_GADGET
    Q_PROPERTY(QString iconSource READ GetIconSource CONSTANT)
    Q_PROPERTY(QString link READ GetLink CONSTANT)

    public:
        QString GetIconSource() const { return IconSource; }
        QString GetLink()       const { return Link; }

        EPlatform Platform = EPlatform::Unknown;
        QString IconSource = "";
        QString Link = "";
};

#endif