#include "Config.h"

void Config::SaveSettings() {
    // Save values to settings
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);

    settings.beginGroup("Output");
    settings.setValue("overwriteEnabled", Overwrite);
    settings.setValue("codecIndex", CodecIndex());
    settings.setValue("normalizeEnabled", NormalizeAudio);
    settings.setValue("normalizeVolume", NormalizeAudioVolume);

    // Audio Bitrate
    for (int i = 0; i < Codec::Data.count(); i++) {
        Codec::Extension currentExtension = (Codec::Extension)i;

        // Dont save if bitrate cannot be set
        if (Codec::Data[currentExtension].LockedBitrate)
            continue;

        // Save bitrate
        QString codecBitrateKey = QString("audioBitrate%1").arg(Codec::Data[currentExtension].String.toUpper());
        settings.setValue(codecBitrateKey, AudioBitrate[currentExtension]);
    }

    settings.setValue("songOutputFormatTag", SongOutputFormatTag);
    settings.setValue("songOutputFormat", SongOutputFormat);
    settings.setValue("folderSortingIndex", FolderSortingIndex);
    settings.endGroup();

    settings.beginGroup("Downloading");
    settings.setValue("statusNotificationsEnabled", Notifications);
    settings.setValue("downloaderThreads", ThreadCount);
    settings.setValue("downloadSpeedLimit", DownloadSpeed);
    settings.endGroup();

    settings.beginGroup("Interface");
    settings.setValue("downloaderThreadUIIndex", DownloaderThreadUIIndex);
    settings.endGroup();

    // Log settings
    QJsonObject settingsLog = QJsonObject {
        {"Overwrite Enabled", Overwrite},
        {"Codec Index", CodecIndex()},
        {"Normalise Enabled", NormalizeAudio},
        {"Normalise Volume", NormalizeAudioVolume},
        {"Song Output Format Tag", SongOutputFormatTag},
        {"Song Output Format", SongOutputFormat},
        {"Folder Sorting Index", FolderSortingIndex},
        {"Status Notifications Enabled", Notifications},
        {"Downloader Threads", ThreadCount},
        {"Download Speed Limit", DownloadSpeed},
        {"Downloader Thread UI Index", DownloaderThreadUIIndex}
    };

    // Add bitrate to log
    for (int i = 0; i < Codec::Data.count(); i++) {
        Codec::Extension currentExtension = (Codec::Extension)i;
        if (Codec::Data[currentExtension].LockedBitrate)
            continue;

        settingsLog.insert(QString("Audio Bitrate %1").arg(Codec::Data[currentExtension].String.toUpper()), AudioBitrate[currentExtension]);
    }

    qInfo() << "Settings Successfully Saved" << settingsLog;
}

void Config::LoadSettings() {
    // Default settings are defined here

    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);

    settings.beginGroup("Output");
    Overwrite = settings.value("overwriteEnabled", false).toBool();
    SetCodecIndex(settings.value("codecIndex", 0).toInt());
    NormalizeAudio = settings.value("normalizeEnabled", true).toBool();
    NormalizeAudioVolume = settings.value("normalizeVolume", -14.0).toFloat();
    
    // Audio Bitrate
    for (int i = 0; i < Codec::Data.count(); i++) {
        // Check if bitrate can be set
        Codec::Extension currentExtension = (Codec::Extension)i;
        if (Codec::Data[currentExtension].LockedBitrate)
            continue;

        // Get current metadata type bitrate
        QString codecBitrateKey = QString("audioBitrate%1").arg(Codec::Data[currentExtension].String.toUpper());
        int audioBitrate = settings.value(codecBitrateKey, NULL).toInt();

        // If bitrate is not assigned, set to default
        if (audioBitrate == NULL) {
            // If MP3 is not assigned and bitrate was set in a previous version, set it to that
            QVariant previousAudioBitrate = settings.value("audioBitrate", NULL);
            if (currentExtension == Codec::Extension::MP3 && previousAudioBitrate != NULL) {
                AudioBitrate[currentExtension] = previousAudioBitrate.toInt();

                // Remove previous value, not needed anymore
                settings.remove("audioBitrate");

                continue;
            }

            // If previous bitrate is null, set to default
            AudioBitrate[currentExtension] = Codec::Data[currentExtension].DefaultBitrate;\
            continue;
        }

        // Set bitrate
        AudioBitrate[currentExtension] = audioBitrate;
    }

    SaveLocation = settings.value("saveLocation", "").toString();
    SongOutputFormatTag = settings.value("songOutputFormatTag", "<>").toString();
    SongOutputFormat = settings.value("songOutputFormat", "<Song Name> - <Song Artist>").toString();
    FolderSortingIndex = settings.value("folderSortingIndex", 0).toInt();
    settings.endGroup();

    settings.beginGroup("Downloading");
    Notifications = settings.value("statusNotificationsEnabled", true).toBool();
    ThreadCount = settings.value("downloaderThreads", 6).toInt();
    DownloadSpeed = settings.value("downloadSpeedLimit", 0.0).toFloat();
    settings.endGroup();

    settings.beginGroup("Interface");
    DownloaderThreadUIIndex = settings.value("downloaderThreadUIIndex", 0).toInt();
    settings.endGroup();

    // Log settings
    QJsonObject settingsLog = QJsonObject{
        {"Overwrite Enabled", Overwrite},
        {"Codec Index", CodecIndex()},
        {"Normalise Enabled", NormalizeAudio},
        {"Normalise Volume", NormalizeAudioVolume},
        {"Song Output Format Tag", SongOutputFormatTag},
        {"Song Output Format", SongOutputFormat},
        {"Folder Sorting Index", FolderSortingIndex},
        {"Status Notifications Enabled", Notifications},
        {"Downloader Threads", ThreadCount},
        {"Download Speed Limit", DownloadSpeed},
        {"Downloader Thread UI Index", DownloaderThreadUIIndex}
    };

    // add bitrate to log
    for (int i = 0; i < Codec::Data.count(); i++) {
        Codec::Extension currentExtension = (Codec::Extension)i;
        if (Codec::Data[currentExtension].LockedBitrate)
            continue;

        settingsLog.insert(QString("Audio Bitrate %1").arg(Codec::Data[currentExtension].String.toUpper()), AudioBitrate[currentExtension]);
    }

    qInfo() << "Settings Successfully Saved" << settingsLog;
}

// Convert naming tags to QStringList, cannot have const QStringList
QStringList Config::Q_NAMING_TAGS() {
    if (!Q_NAMING_TAGS_CACHE.isEmpty())
        return Q_NAMING_TAGS_CACHE;

    QStringList tags = QStringList();
    int numTags = std::extent<decltype(NAMING_TAGS)>::value;
    for (int i = 0; i < numTags; i++) {
        QString tag = QString::fromUtf8(NAMING_TAGS[i]);
        tags.append(tag);
    }

    Q_NAMING_TAGS_CACHE = tags;
    return tags;
}

std::tuple<QString, Config::NamingError> Config::FormatOutputNameWithTags(std::function<QString(QString)> tagHandlerFunc) {
    QString songOutputFormatTag = SongOutputFormatTag;
    QString songOutputFormat = SongOutputFormat;

    if (songOutputFormatTag.length() != 2) {
        return std::make_tuple(songOutputFormatTag, NamingError::EnclosingTagsInvalid);
    }

    QChar leftTag = songOutputFormatTag[0];
    QChar rightTag = songOutputFormatTag[1];

    QStringList namingTags = Q_NAMING_TAGS();

    QString newString;
    int currentCharIndex = 0;
    while (currentCharIndex <= songOutputFormat.length()) {
        int nextLeftIndex = songOutputFormat.indexOf(leftTag, currentCharIndex);
        int nextRightIndex = songOutputFormat.indexOf(rightTag, currentCharIndex);
        int tagLength = nextRightIndex - nextLeftIndex - 1;
        QString tag = songOutputFormat.mid(nextLeftIndex + 1, tagLength);

        if (nextLeftIndex == -1 || nextRightIndex == -1) {
            QString afterTagString = songOutputFormat.mid(currentCharIndex, songOutputFormat.length() - currentCharIndex);
            newString.append(afterTagString);

            break;
        }

        QString beforeTagString = songOutputFormat.mid(currentCharIndex, nextLeftIndex - currentCharIndex);
        newString.append(beforeTagString);

        QString tagReplacement = tagHandlerFunc(tag);
        if (tagReplacement.isNull()) {
            return std::make_tuple(tag, NamingError::TagInvalid);
        }

        newString.append(tagReplacement);

        currentCharIndex = nextRightIndex + 1;
    }

    return std::make_tuple(newString, NamingError::None);
}