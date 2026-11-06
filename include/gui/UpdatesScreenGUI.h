#ifndef UPDATESSCREENGUI_H
#define UPDATESSCREENGUI_H

#include "DependencyInfo.h"

#include <QObject>
#include <QMap>
#include <QtQml/qqml.h>

class UpdatesScreenGUI : public QObject
{
    Q_OBJECT
    Q_PROPERTY(DependencyInfo* selectedDependency READ GetSelectedDependency NOTIFY SelectedDependencyChanged)

    public:
        explicit UpdatesScreenGUI(QObject* parent = 0);

        Q_INVOKABLE void SelectDependency(const DependencyInfo::EDependency& dependency);

        Q_INVOKABLE DependencyInfo* GetDependency(const DependencyInfo::EDependency& dependency) const;
        Q_INVOKABLE DependencyInfo* GetSelectedDependency() const;
    private:
        void CreateDependency(const DependencyInfo::EDependency& type, const QString& name, const QString& iconSource, const QString& version);
    private:
        DependencyInfo::EDependency _selectedDependencyType;

        QMap<DependencyInfo::EDependency, DependencyInfo*> _dependencies;
    signals:
        void SelectedDependencyChanged();
};

#endif