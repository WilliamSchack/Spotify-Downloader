#ifndef DEPENDENCYINFO_H
#define DEPENDENCYINFO_H

#include "EasyQProperties.h"

#include <functional>
#include <thread>

#include <QObject>
#include <QMetaObject>

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
    SIMPLE_QPROPERTY(bool, updateAvailable, UpdateAvailable, _updateAvailable)
    SIMPLE_QPROPERTY(bool, checkingForUpdates, CheckingForUpdates, _checkingForUpdates)

    public:
        explicit DependencyInfo(std::function<std::string()> getVersionFunction, std::function<bool()> updateAvailableFunction, std::function<bool()> updateFunction, QObject* parent);

        void CheckForUpdate();
        void DownloadUpdate();
    private:
        void UpdateVersion();
    private:
        std::function<std::string()> _getVersionFunction;
        std::function<bool()> _updateAvailableFunction;
        std::function<bool()> _updateFunction;

        EDependency _type = EDependency::Unknown;
        QString _name = "";
        QString _version = "";
        QString _iconSource = "";

        bool _updateAvailable = false;
        bool _checkingForUpdates = false;
};

#endif