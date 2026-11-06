#ifndef DEPENDENCYINFO_H
#define DEPENDENCYINFO_H

#include "EasyQProperties.h"

#include <QObject>

class DependencyInfo : public QObject
{
    Q_OBJECT

    public:
        enum class EDependency
        {
            Unknown,
            Echolite,
            Ffmpeg,
            Ytdlp,
            Quickjs
        };
        Q_ENUM(EDependency)

    SIMPLE_QPROPERTY(EDependency, type, Type, _type)
    SIMPLE_QPROPERTY(QString, name, Name, _name)
    SIMPLE_QPROPERTY(QString, version, Version, _version)
    SIMPLE_QPROPERTY(QString, iconSource, IconSource, _iconSource)
    SIMPLE_QPROPERTY(bool, upToDate, UpToDate, _upToDate)

    public:
        explicit DependencyInfo(QObject* parent);
    private:
        EDependency _type = EDependency::Unknown;
        QString _name = "";
        QString _version = "";
        QString _iconSource = "";
        bool _upToDate = true;
};

#endif