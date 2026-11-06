#ifndef UPDATESSCREENGUI_H
#define UPDATESSCREENGUI_H

#include "DependencyInfo.h"

#include <QObject>
#include <QMap>

class UpdatesScreenGUI : public QObject
{
    Q_OBJECT

    public:
        explicit UpdatesScreenGUI(QObject* parent = 0);

        Q_INVOKABLE DependencyInfo* GetDependency(const DependencyInfo::EDependency& dependency) const;
    private:
        void CreateDependency(const DependencyInfo::EDependency& type, const QString& name, const QString& iconSource, const QString& version);
    private:
        QMap<DependencyInfo::EDependency, DependencyInfo*> _dependencies;
};

#endif