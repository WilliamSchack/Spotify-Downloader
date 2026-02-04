#ifndef MAINSCREENGUIMANAGER_H
#define MAINSCREENGUIMANAGER_H

#include <iostream>

#include <QQmlContext>
#include <QObject>
#include <QFileDialog>
#include <QProcess>
#include <QStringList>

#include <taglib/fileref.h>
#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>

class MainScreenGUIManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString folderInputText READ FolderInputText WRITE SetFolderInputText NOTIFY FolderInputTextChanged)

    public:
        explicit MainScreenGUIManager(QObject* parent = 0);
        void SetQmlContext(QQmlContext* qmlContext);
    public:
        QString FolderInputText() const;
        void SetFolderInputText(const QString& text);
    public slots:
        void folderButtonClicked();
        void ytdlpButtonClicked();
        void ffmpegButtonClicked();
        void taglibButtonClicked();
    private:
        inline static constexpr const char* OUTPUT_FILE_NAME = "TestFile";

        QQmlContext* _qmlContext = nullptr;
        QString _folderInputText = "";

        QString _currentDownloadPath = "";
    signals:
        void FolderInputTextChanged();
};

#endif