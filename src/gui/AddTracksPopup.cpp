#include "AddTracksPopup.h"

AddTracksPopup::AddTracksPopup(QWindow* mainWindow, QObject* parent) : _mainWindow(mainWindow), QObject(parent) {}

bool AddTracksPopup::GetVisible() const
{
    return _visible;
}

void AddTracksPopup::SetVisible(const bool& visible)
{
    if (visible == _visible)
        return;
    
    _visible = visible;
    emit VisibleChanged();
}

QString AddTracksPopup::GetLinkInputText() const
{
    return _linkInputText;
}

void AddTracksPopup::SetLinkInputText(const QString& text)
{
    if (text == _linkInputText)
        return;
    
    _linkInputText = text;
    emit LinkInputTextChanged();
}

QString AddTracksPopup::GetFolderInputText() const
{
    return _folderInputText;
}

void AddTracksPopup::SetFolderInputText(const QString& text)
{
    if (text == _folderInputText)
        return;
    
    _folderInputText = text;
    emit FolderInputTextChanged();
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
    dialog.setOptions(QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog);
    FileDialogUtils::SetParent(dialog, _mainWindow);

    if (dialog.exec() == QDialog::Accepted) {
        QString folder = dialog.selectedFiles().first();
        if (!folder.isEmpty())
            SetFolderInputText(folder);
    }
}

void AddTracksPopup::DownloadButtonClicked()
{
    emit DownloadRequested(_linkInputText.toStdString(), _folderInputText.toStdString());
}