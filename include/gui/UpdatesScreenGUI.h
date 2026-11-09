#ifndef UPDATESSCREENGUI_H
#define UPDATESSCREENGUI_H

#include "DependencyInfo.h"
#include "EasyQProperties.h"

#include "VersionManager.h"
#include "Ffmpeg.h"
#include "Ytdlp.h"
#include "JsRuntime.h"

#include <thread>

#include <QObject>
#include <QMap>
#include <QMetaObject>
#include <QDesktopServices>
#include <QtQml/qqml.h>

class UpdatesScreenGUI : public QObject
{
    Q_OBJECT
    Q_PROPERTY(DependencyInfo* selectedDependency READ GetSelectedDependency NOTIFY SelectedDependencyChanged)
    SIMPLE_QPROPERTY(QString, updateButtonText, UpdateButtonText, _updateButtonText)
    SIMPLE_QPROPERTY(bool, checkingForUpdates, CheckingForUpdates, _checkingForUpdates)

    public:
        explicit UpdatesScreenGUI(QObject* parent = 0);

        Q_INVOKABLE void SelectDependency(const DependencyInfo::EDependency& dependency);

        Q_INVOKABLE DependencyInfo* GetDependency(const DependencyInfo::EDependency& dependency) const;
        Q_INVOKABLE DependencyInfo* GetSelectedDependency() const;
    public slots:
        void CheckForUpdatesButtonClicked();
        void SourceButtonClicked();
    private:
        void CreateDependency(const DependencyInfo::EDependency& type, const QString& name, const QString& version, const QString& iconSource);
    private:
        DependencyInfo::EDependency _selectedDependencyType;

        QMap<DependencyInfo::EDependency, DependencyInfo*> _dependencies;

        QString _updateButtonText = "Check for Updates";
        bool _checkingForUpdates = false;
    signals:
        void SelectedDependencyChanged();
};

#endif