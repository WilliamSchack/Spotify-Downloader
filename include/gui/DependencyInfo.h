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
    SIMPLE_QPROPERTY(bool, installed, Installed, _installed)
    SIMPLE_QPROPERTY(bool, installedOnSystem, InstalledOnSystem, _installedOnSystem)
    SIMPLE_QPROPERTY(bool, updateAvailable, UpdateAvailable, _updateAvailable)
    SIMPLE_QPROPERTY(bool, checkingForUpdates, CheckingForUpdates, _checkingForUpdates)
    SIMPLE_QPROPERTY(bool, updating, Updating, _updating)
    SIMPLE_QPROPERTY(bool, downloading, Downloading, _downloading)

    public:
        explicit DependencyInfo(std::function<bool()> installedFunction, std::function<bool()> installedOnSystemFunction, std::function<std::string()> getVersionFunction, std::function<bool()> updateAvailableFunction, std::function<bool()> updateFunction, std::function<bool()> downloadFunction, QObject* parent);

        void CheckInstalled();
        void CheckForUpdate();
        void DownloadUpdate();
        void Download();
    private:
        void UpdateVersion();
    private:
        std::function<bool()> _installedFunction;
        std::function<bool()> _installedOnSystemFunction;
        std::function<std::string()> _getVersionFunction;
        std::function<bool()> _updateAvailableFunction;
        std::function<bool()> _updateFunction;
        std::function<bool()> _downloadFunction;

        EDependency _type = EDependency::Unknown;
        QString _name = "";
        QString _version = "";
        QString _iconSource = "";

        bool _installed = true;
        bool _installedOnSystem = true;
        bool _updateAvailable = false;
        bool _checkingForUpdates = false;
        bool _updating = false;
        bool _downloading = false;
};

#endif