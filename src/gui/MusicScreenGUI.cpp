#include "MusicScreenGUI.h"

MusicScreenGUI::MusicScreenGUI(QObject* parent) : QObject(parent)
{
    _addTracksPopup = new AddTracksPopup(this);
    connect(_addTracksPopup, &AddTracksPopup::DownloadRequested, this, &MusicScreenGUI::OnDownloadRequested);
}

AddTracksPopup* MusicScreenGUI::GetAddTracksPopup() const
{
    return _addTracksPopup;
}

void MusicScreenGUI::OnDownloadRequested(const std::string& link, const std::string& destinationFolder)
{
    std::cout << link << " || " << destinationFolder << std::endl;
}