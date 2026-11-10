#ifndef INFOSCREENGUI_H
#define INFOSCREENGUI_H

#include "UpdatesScreenGUI.h"
#include "NoticesScreenGUI.h"

#include <QObject>

class InfoScreenGUI : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject* updates READ GetUpdatesManager CONSTANT)
    Q_PROPERTY(QObject* notices READ GetNoticesManager CONSTANT)
    
    public:
        explicit InfoScreenGUI(QObject* parent = 0);

        UpdatesScreenGUI* GetUpdatesManager() const;
        NoticesScreenGUI* GetNoticesManager() const;
    private:
        UpdatesScreenGUI* _updatesManager;
        NoticesScreenGUI* _noticesManager;
};

#endif