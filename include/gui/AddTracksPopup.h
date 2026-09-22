#ifndef ADDTRACKSPOPUP_H
#define ADDTRACKSPOPUP_H

#include <functional>
#include <string>

#include <QApplication>
#include <QFileDialog>
#include <QClipboard>

class AddTracksPopup : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString folderInputText READ GetFolderInputText WRITE SetFolderInputText NOTIFY FolderInputTextChanged)
    Q_PROPERTY(QString linkInputText READ GetLinkInputText WRITE SetLinkInputText NOTIFY LinkInputTextChanged)

    public:
        explicit AddTracksPopup(QObject* parent = 0);

        QString GetLinkInputText() const;
        void SetLinkInputText(const QString& text);

        QString GetFolderInputText() const;
        void SetFolderInputText(const QString& text);
    public slots:
        void PasteButtonClicked();
        void FolderButtonClicked();
        void DownloadButtonClicked();
    private:
        QString _linkInputText = "";
        QString _folderInputText = "";
    signals:
        void LinkInputTextChanged();
        void FolderInputTextChanged();
        void DownloadRequested(const std::string& link, const std::string& destinationFolder);
};

#endif