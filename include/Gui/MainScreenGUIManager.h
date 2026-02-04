#ifndef MAINSCREENGUIMANAGER_H
#define MAINSCREENGUIMANAGER_H

#include <iostream>

#include <QStringList>
#include <QQmlContext>
#include <QFileDialog>
#include <QProcess>
#include <QClipboard>
#include <QApplication>

#include <taglib/fileref.h>
#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>

class MainScreenGUIManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString folderInputText READ FolderInputText WRITE SetFolderInputText NOTIFY FolderInputTextChanged)
    Q_PROPERTY(QString linkInputText READ LinkInputText WRITE SetLinkInputText NOTIFY LinkInputTextChanged)

    public:
        explicit MainScreenGUIManager(QObject* parent = 0);
    public:
        QString FolderInputText() const;
        void SetFolderInputText(const QString& text);

        QString LinkInputText() const;
        void SetLinkInputText(const QString& text);
    public slots:
        void PasteButtonClicked();
        void FolderButtonClicked();
        void DownloadButtonClicked();
    private:
        QString _folderInputText = "";
        QString _linkInputText = "";
    signals:
        void FolderInputTextChanged();
        void LinkInputTextChanged();
};

#endif