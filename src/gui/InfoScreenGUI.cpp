#include "InfoScreenGUI.h"

InfoScreenGUI::InfoScreenGUI(QObject* parent) : QObject(parent)
{
    _updatesManager = new UpdatesScreenGUI(this);
    _noticesManager = new NoticesScreenGUI(this);
    _supportersManager = new SupportersScreenGUI(this);
}

UpdatesScreenGUI* InfoScreenGUI::GetUpdatesManager() const
{
    return _updatesManager;
}

NoticesScreenGUI* InfoScreenGUI::GetNoticesManager() const
{
    return _noticesManager;
}

SupportersScreenGUI* InfoScreenGUI::GetSupportersManager() const
{
    return _supportersManager;
}