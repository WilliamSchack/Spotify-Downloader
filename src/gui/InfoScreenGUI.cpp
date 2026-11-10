#include "InfoScreenGUI.h"

InfoScreenGUI::InfoScreenGUI(QObject* parent) : QObject(parent)
{
    _updatesManager = new UpdatesScreenGUI(this);
    _noticesManager = new NoticesScreenGUI(this);
}

UpdatesScreenGUI* InfoScreenGUI::GetUpdatesManager() const
{
    return _updatesManager;
}

NoticesScreenGUI* InfoScreenGUI::GetNoticesManager() const
{
    return _noticesManager;
}