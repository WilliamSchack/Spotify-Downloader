#include "MainScreenGUIManager.h"

MainScreenGUIManager::MainScreenGUIManager(QObject* parent) : QObject(parent) {}

QString MainScreenGUIManager::LinkInputText() const
{
    return _linkInputText;
}

void MainScreenGUIManager::SetLinkInputText(const QString& text)
{
    if (text == _linkInputText)
        return;
    
    _linkInputText = text;
    emit LinkInputTextChanged();
}

QString MainScreenGUIManager::FolderInputText() const
{
    return _folderInputText;
}

void MainScreenGUIManager::SetFolderInputText(const QString& text)
{
    if (text == _folderInputText)
        return;
    
    _folderInputText = text;
    emit FolderInputTextChanged();
}

void MainScreenGUIManager::PasteButtonClicked()
{
    const QClipboard* clipboard = QApplication::clipboard();
    QString clipboardText = clipboard->text();

    if (!clipboardText.isEmpty())
        SetLinkInputText(clipboardText);
}

void MainScreenGUIManager::FolderButtonClicked()
{
    QString folder = QFileDialog::getExistingDirectory(nullptr, "Choose test directory", "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!folder.isEmpty())
        SetFolderInputText(folder);
}

void MainScreenGUIManager::DownloadButtonClicked()
{
    DownloadManager::Download(_linkInputText.toStdString());
}