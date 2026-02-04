#include "MainScreenGUIManager.h"

MainScreenGUIManager::MainScreenGUIManager(QObject* parent) : QObject(parent) {}

void MainScreenGUIManager::SetQmlContext(QQmlContext* qmlContext) {
    _qmlContext = qmlContext;
}

QString MainScreenGUIManager::FolderInputText() const {
    return _folderInputText;
}

void MainScreenGUIManager::SetFolderInputText(const QString& text) {
    if (text == _folderInputText)
        return;
    
    _folderInputText = text;
    emit FolderInputTextChanged();
}

void MainScreenGUIManager::folderButtonClicked() {
    QString folder = QFileDialog::getExistingDirectory(nullptr, "Choose test directory", "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!folder.isEmpty())
        SetFolderInputText(folder);
}

void MainScreenGUIManager::ytdlpButtonClicked() {
    const QString downloadPathNoExtension = QString("%1/%2").arg(_folderInputText).arg(OUTPUT_FILE_NAME);

    QStringList arguments;
    arguments << "--ffmpeg-location" << FFMPEG_PATH;
    arguments << "--extractor-args" << "youtube:player_client:default";
    arguments << "-v" << "--no-part" << "--no-simulate";
    arguments << "-f" << "ba/b";
    arguments << "--audio-quality" << "0";
    arguments << "--print" << "%(ext)s";
    arguments << "-o" << QString("%1.%(ext)s").arg(downloadPathNoExtension);
    arguments << QString::fromStdString("https://www.youtube.com/watch?v=YcO-MxPf_Vg");

    QProcess* process = new QProcess();
    QObject::connect(process, &QProcess::finished, process, &QProcess::deleteLater);

    QString extension = "";
    QObject::connect(process, &QProcess::readyRead, process, [&process, &extension] {
        QString output = process->readAll();
        extension = output.split("\n")[0];
    });

    process->start(YTDLP_PATH, arguments);
    process->waitForFinished(-1);

    qInfo() << "yt-dlp Finished";
    qInfo() << process->readAllStandardError();

    _currentDownloadPath = QString("%1.%3").arg(downloadPathNoExtension).arg(extension);
}

void MainScreenGUIManager::ffmpegButtonClicked() {
    const QString originalFilePath = _currentDownloadPath;

    int lastPeriodIndex = originalFilePath.lastIndexOf(".");
    _currentDownloadPath = originalFilePath.mid(0, lastPeriodIndex) + ".mp3";

    QStringList arguments;
    arguments << "-i" << originalFilePath;
    arguments << "-nostats";
    arguments << "-acodec" << "libmp3lame";
    arguments << _currentDownloadPath;

    QProcess* process = new QProcess();
    QObject::connect(process, &QProcess::finished, process, &QProcess::deleteLater);
    process->start(FFMPEG_PATH, arguments);
    process->waitForFinished(-1);

    QFile(originalFilePath).remove();
}

void MainScreenGUIManager::taglibButtonClicked() {
    TagLib::FileRef tagFileRef(_currentDownloadPath.toStdString().c_str(), true, TagLib::AudioProperties::Accurate);
    TagLib::MPEG::File* tagMpegFile = dynamic_cast<TagLib::MPEG::File*>(tagFileRef.file());

    TagLib::ID3v2::Tag* tag = tagMpegFile->ID3v2Tag();

    tag->setTitle("CRAZYYYY TITLLLLE");
    tag->setArtist("ARTISSST");
    tag->setAlbum("album");

    tagFileRef.save();
}