#include "AddTracksPopup.h"

AddTracksPopup::AddTracksPopup(QObject* parent) : QObject(parent) {}

void AddTracksPopup::PasteButtonClicked()
{
    const QClipboard* clipboard = QApplication::clipboard();
    QString clipboardText = clipboard->text();

    if (!clipboardText.isEmpty())
        SetLinkInputText(clipboardText);
}

void AddTracksPopup::FolderButtonClicked()
{
    // TODO: Test this on different operating systems, dialog looks shit on windows

    QFileDialog dialog(nullptr, "Choose output directory", "");
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOptions(QFileDialog::ShowDirsOnly);
    FileDialogUtils::SetParent(dialog, QGuiApplication::focusWindow());

    if (dialog.exec() != QDialog::Accepted)
        return;
        
    QString folder = dialog.selectedFiles().first();
    if (folder.isEmpty())
        return;
    
    SetFolderInputText(folder);
}

void AddTracksPopup::DownloadButtonClicked()
{
    emit DownloadRequested(_linkInputText.toStdString(), _folderInputText.toStdString());
}