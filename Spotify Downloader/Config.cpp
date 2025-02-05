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

    settings.setValue("songOutputFormatTag", FileNameTag);
    settings.setValue("songOutputFormat", FileName);
    settings.setValue("subFoldersTag", SubFoldersTag);
    settings.setValue("subFolders", SubFolders);
    settings.endGroup();

    settings.beginGroup("Downloading");
    settings.setValue("statusNotificationsEnabled", Notifications);
    settings.setValue("downloaderThreads", ThreadCount);
    settings.setValue("downloadSpeedLimit", DownloadSpeed);
    settings.endGroup();

    settings.beginGroup("Interface");
    settings.setValue("downloaderThreadUIIndex", DownloaderThreadUIIndex);
    settings.setValue("sidebarIconsColour", SidebarIconsColour);
    settings.setValue("checkForUpdates", CheckForUpdates);
    settings.endGroup();

    // Log settings
    qInfo() << "Settings Successfully Saved" << SettingsLog();
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
    FileNameTag = settings.value("songOutputFormatTag", "<>").toString();
    FileName = settings.value("songOutputFormat", "<Song Name> - <Song Artist>").toString();

    // Folder Sorting
    SubFoldersTag = settings.value("subFoldersTag", "<>").toString();
    QString subFolders = settings.value("subFolders", "").toString();

    // Check if folder sorting index was set in previous version
    QVariant previousFolderSortingIndex = settings.value("folderSortingIndex", NULL);
    if (previousFolderSortingIndex != NULL) {
        // If it was set, assign new folder sorting to it
        int folderSortingIndex = previousFolderSortingIndex.toInt();

        switch (folderSortingIndex) {
            // 0 is None
            case 1: // Album Name
                subFolders = "<Album Name>";
                break;
            case 2: // Song Artist
                subFolders = "<Song Artist>";
                break;
            case 3: // Song Artists
                subFolders = "<Song Artists>";
                break;
            case 4: // Album Artist
                subFolders = "<Album Artist>";
                break;
            case 5: // Album Artists
                subFolders = "<Album Artists>";
                break;
        }

        // Remove previous value, not needed anymore
        settings.remove("folderSortingIndex");
    }

    SubFolders = subFolders;

    settings.endGroup();

    settings.beginGroup("Downloading");
    Notifications = settings.value("statusNotificationsEnabled", true).toBool();
    ThreadCount = settings.value("downloaderThreads", 6).toInt();
    DownloadSpeed = settings.value("downloadSpeedLimit", 0.0).toFloat();
    settings.endGroup();

    settings.beginGroup("Interface");
    DownloaderThreadUIIndex = settings.value("downloaderThreadUIIndex", 0).toInt();
    SidebarIconsColour = settings.value("sidebarIconsColour", true).toBool();
    CheckForUpdates = settings.value("checkForUpdates", true).toBool();
    settings.endGroup();

    // Log settings
    qInfo() << "Settings Successfully Saved" << SettingsLog();
}

QJsonObject Config::SettingsLog() {
    // Log settings
    QJsonObject settingsLog = QJsonObject{
        {"Overwrite Enabled", Overwrite},
        {"Codec Index", CodecIndex()},
        {"Normalise Enabled", NormalizeAudio},
        {"Normalise Volume", NormalizeAudioVolume},
        {"File Name Tag", FileNameTag},
        {"File Name Format", FileName},
        {"Sub Folders Tag", SubFoldersTag},
        {"Sub Folders", SubFolders},
        {"Status Notifications Enabled", Notifications},
        {"Downloader Threads", ThreadCount},
        {"Download Speed Limit", DownloadSpeed},
        {"Downloader Thread UI Index", DownloaderThreadUIIndex},
        {"Sidebar Icons Colour", SidebarIconsColour},
        {"Check For Updates", CheckForUpdates}
    };

    // add bitrate to log
    for (int i = 0; i < Codec::Data.count(); i++) {
        Codec::Extension currentExtension = (Codec::Extension)i;
        if (Codec::Data[currentExtension].LockedBitrate)
            continue;

        settingsLog.insert(QString("Audio Bitrate %1").arg(Codec::Data[currentExtension].String.toUpper()), AudioBitrate[currentExtension]);
    }

    return settingsLog;
}

std::tuple<QString, Config::NamingError> Config::FormatStringWithTags(QString stringTag, QString string, std::function<std::tuple<QString, bool>(QString)> tagHandlerFunc) {
    if (stringTag.length() != 2) {
        return std::make_tuple(stringTag, NamingError::EnclosingTagsInvalid);
    }

    QChar leftTag = stringTag[0];
    QChar rightTag = stringTag[1];

    QString newString;
    int currentCharIndex = 0;
    while (currentCharIndex <= string.length()) {
        int nextLeftIndex = string.indexOf(leftTag, currentCharIndex);
        int nextRightIndex = string.indexOf(rightTag, currentCharIndex);
        int tagLength = nextRightIndex - nextLeftIndex - 1;
        QString tag = string.mid(nextLeftIndex + 1, tagLength);

        if (nextLeftIndex == -1 || nextRightIndex == -1) {
            QString afterTagString = string.mid(currentCharIndex, string.length() - currentCharIndex);
            newString.append(afterTagString);

            break;
        }

        QString beforeTagString = string.mid(currentCharIndex, nextLeftIndex - currentCharIndex);
        newString.append(beforeTagString);

        std::tuple<QString, bool> tagReplacementReturn = tagHandlerFunc(tag);

        // If right value is false, tag replacement was not returned
        if (!std::get<1>(tagReplacementReturn)) {
            return std::make_tuple(tag, NamingError::TagInvalid);
        }

        QString tagReplacement = std::get<0>(tagReplacementReturn);


        newString.append(tagReplacement);

        currentCharIndex = nextRightIndex + 1;
    }

    return std::make_tuple(newString, NamingError::None);
}