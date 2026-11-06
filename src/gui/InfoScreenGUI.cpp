#include "InfoScreenGUI.h"

InfoScreenGUI::InfoScreenGUI(QObject* parent) : QObject(parent)
{
    _updatesManager = new UpdatesScreenGUI(this);
}

UpdatesScreenGUI* InfoScreenGUI::GetUpdatesManager() const
{
    return _updatesManager;
}