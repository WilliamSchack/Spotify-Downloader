#ifndef GUIRELEASEINFO_H
#define GUIRELEASEINFO_H

#include <QObject>

struct GUIReleaseInfo
{
    Q_GADGET
    Q_PROPERTY(QString name READ GetName CONSTANT)
    Q_PROPERTY(QString body READ GetBody CONSTANT)

    public:
        QString GetName() const { return Name; }
        QString GetBody() const { return Body; }

        QString Name = "";
        QString Body = "";
};

#endif