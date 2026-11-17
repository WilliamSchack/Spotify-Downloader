#include "AddTracksPopup.h"

AddTracksPopup::AddTracksPopup(QObject* parent) : QObject(parent)
{
    if (!Config::DefaultSaveLocation.empty() && std::filesystem::exists(Config::DefaultSaveLocation))
        _folderInputText = QString::fromStdString(FileUtils::PathToUtf8(Config::DefaultSaveLocation));
}

void AddTracksPopup::SetError(const std::string& errorString)
{
    if (errorString.empty())
        return;

    emit startError(QString::fromStdString(errorString));
}

void AddTracksPopup::PasteButtonClicked()
{
    const QClipboard* clipboard = QApplication::clipboard();
    QString clipboardText = clipboard->text();

    if (!clipboardText.isEmpty())
        SetLinkInputText(clipboardText);
}

void AddTracksPopup::FolderButtonClicked()
{
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