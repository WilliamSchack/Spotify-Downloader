#ifndef ADDTRACKSPOPUP_H
#define ADDTRACKSPOPUP_H

#include "FileDialogUtils.h"

#include <functional>
#include <string>

#include <QApplication>
#include <QClipboard>

class AddTracksPopup : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool visible READ GetVisible WRITE SetVisible NOTIFY VisibleChanged)
    Q_PROPERTY(QString folderInputText READ GetFolderInputText WRITE SetFolderInputText NOTIFY FolderInputTextChanged)
    Q_PROPERTY(QString linkInputText READ GetLinkInputText WRITE SetLinkInputText NOTIFY LinkInputTextChanged)

    public:
        explicit AddTracksPopup(QWindow* mainWindow, QObject* parent = 0);

        bool GetVisible() const;
        void SetVisible(const bool& visible);

        QString GetLinkInputText() const;
        void SetLinkInputText(const QString& text);

        QString GetFolderInputText() const;
        void SetFolderInputText(const QString& text);
    public slots:
        void PasteButtonClicked();
        void FolderButtonClicked();
        void DownloadButtonClicked();
    private:
        QWindow* _mainWindow;

        bool _visible = true;

        QString _linkInputText = "";
        QString _folderInputText = "";
    signals:
        void VisibleChanged();
        void LinkInputTextChanged();
        void FolderInputTextChanged();
        void DownloadRequested(const std::string& link, const std::string& destinationFolder);
};

#endif