#include "Config.h"

nlohmann::ordered_json Config::ToJson()
{
    // Store enums as strings incase the order changes later

    nlohmann::ordered_json json = {
        { "output", {
            { "audio", {
                { "codecExtension", EEXTENSION_NAMES[(int)CodecExtension] },
                { "normalise", Normalise },
                { "normaliseDb", NormaliseDb },
                { "manualBitrate", ManualBitrate },
                { "bitrate", nlohmann::ordered_json::object() } // Filled later
            }},
            { "metadata", {
                { "getLyrics", GetLyrics },
                { "artistSeparator", ArtistSeparator },
                { "trackNumberType", ETRACKNUMBERTYPE_NAMES[(int)TrackNumberType] }
            }},
            { "fileManagement", {
                { "overwrite", Overwrite },
                { "defaultSaveLocation", DefaultSaveLocation },
                { "fileName", {
                    { "tagsOpening", std::string(1, FileNameTagsOpeningChar) },
                    { "tagsClosing", std::string(1, FileNameTagsClosingChar) },
                    { "value", FileName }
                }},
                { "subFolders", {
                    { "tagsOpening", std::string(1, SubFoldersTagsOpeningChar) },
                    { "tagsClosing", std::string(1, SubFoldersTagsClosingChar) },
                    { "value", SubFolders }
                }}
            }},
            { "lyricsFile", {
                { "createLyricsFile", CreateLyricsFile },
                { "lyricsFileName", {
                    { "tagsOpening", std::string(1, LyricsFileNameTagsOpeningChar) },
                    { "tagsClosing", std::string(1, LyricsFileNameTagsClosingChar) },
                    { "value", LyricsFileName }
                }}
            }},
            { "playlistFile", {
                { "createPlaylistFile", CreatePlaylistFile },
                { "playlistFileType", EPLAYLISTFILETYPE_NAMES[(int)PlaylistFileType] },
                { "playlistFileName", {
                    { "tagsOpening", std::string(1, PlaylistFileNameTagsOpeningChar) },
                    { "tagsClosing", std::string(1, PlaylistFileNameTagsClosingChar) },
                    { "value", PlaylistFileName }
                }}
            }}
        }},
        { "downloading", {
            { "general", {
                { "perDownloadThreads", PerDownloadThreads },
                { "downloadSpeedLimit", DownloadSpeedLimit },
                { "downloadTimeoutMs", DownloadTimeoutMs },
                { "ipVersion", EIPVERSION_NAMES[(int)IPVersion] }
            }}
        }},
        { "platforms", {
            { "youtube", {
                { "poToken", YouTubePOToken }
            }}
        }},
        { "interface", {
            { "general", {
                { "showStatusNotifications", ShowStatusNotifications },
                { "autoOpenDownloadFolder", AutoOpenDownloadFolder }
            }},
            { "updates", {
                { "autoCheckForUpdates", AutoCheckForUpdates },
                { "autoCheckForNotices", AutoCheckForNotices }
            }},
            { "background", {
                { "randomDefaultBackground", RandomDefaultBackground },
                { "defaultBackgroundUrl", DefaultBackgroundUrl }
            }}
        }}
    };

    for (const auto& [key, val] : BitrateKbps) {
        std::string keyString = EEXTENSION_NAMES[(int)key];
        json["output"]["audio"]["bitrate"][keyString] = val;
    }

    for (const auto& [key, val] : LosslessFormats) {
        std::string keyString = EEXTENSION_NAMES[(int)key];
        json["output"]["audio"]["losslessFormats"][keyString] = ELOSSLESSCODECFORMAT_NAMES[(int)val];
    }

    return json;
}

void Config::FromJson(const nlohmann::json& json)
{\
    // Does validating for everything incase the file was modified outside the program

    // Helpers
    auto readTaggableInputJson = [&json](const std::vector<std::variant<std::string, int>>& keys, char& tagsOpeningChar, char& tagsClosingChar, std::string& value) {
        nlohmann::json parentJson = JsonUtils::SafelyNavigate(json, keys);
        if (parentJson.empty())
            return;

        std::string tagsOpening = JsonUtils::SafelyNavigate<std::string>(parentJson, { "tagsOpening" }, std::string(1, tagsOpeningChar));
        if (tagsOpening.size() == 1)
            tagsOpeningChar = tagsOpening[0];

        std::string tagsClosing = JsonUtils::SafelyNavigate<std::string>(parentJson, { "tagsClosing" }, std::string(1, tagsClosingChar));
        if (tagsClosing.size() == 1)
            tagsClosingChar = tagsClosing[0];

        value = JsonUtils::SafelyNavigate<std::string>(parentJson, { "value" }, value);
    };

    // == Output

    // Audio
    CodecExtension = JsonUtils::ReadStringEnum<EExtension>(json, { "output", "audio", "codecExtension" }, EEXTENSION_NAMES, CodecExtension);
    Normalise = JsonUtils::SafelyNavigate<bool>(json, { "output", "audio", "normalise" }, Normalise);
    NormaliseDb = JsonUtils::SafelyNavigate<double>(json, { "output", "audio", "normaliseDb" }, NormaliseDb);
    ManualBitrate = JsonUtils::SafelyNavigate<bool>(json, { "output", "audio", "manualBitrate" }, ManualBitrate);

    nlohmann::json bitrateJson = JsonUtils::SafelyNavigate(json, { "output", "audio", "bitrate" });
    for (const auto& [key, val] : bitrateJson.items()) {
        if (!val.is_string())
            continue;

        int indexOfExtension = VectorUtils::IndexOf(EEXTENSION_NAMES, key);
        if (indexOfExtension < 0)
            continue;

        EExtension extension = static_cast<EExtension>(indexOfExtension);
        BitrateKbps[extension] = val.get<int>();
    }

    nlohmann::json losslessFormatsJson = JsonUtils::SafelyNavigate(json, { "output", "audio", "losslessFormats" });
    for (const auto& [key, val] : losslessFormatsJson.items()) {
        if (!val.is_string())
            continue;

        int indexOfExtension = VectorUtils::IndexOf(EEXTENSION_NAMES, key);
        if (indexOfExtension < 0)
            continue;

        EExtension extension = static_cast<EExtension>(indexOfExtension);

        int indexOfFormat = VectorUtils::IndexOf(ELOSSLESSCODECFORMAT_NAMES, val.get<std::string>());
        if (indexOfFormat < 0)
            continue;

        ELosslessCodecFormat format = static_cast<ELosslessCodecFormat>(indexOfFormat);

        LosslessFormats[extension] = format;
    }

    // Metadata
    GetLyrics = JsonUtils::SafelyNavigate<bool>(json, { "output", "metadata", "getLyrics" }, GetLyrics);
    ArtistSeparator = JsonUtils::SafelyNavigate<std::string>(json, { "output", "metadata", "artistSeparator" }, ArtistSeparator);
    TrackNumberType = JsonUtils::ReadStringEnum<ETrackNumberType>(json, { "output", "metadata", "trackNumberType" }, ETRACKNUMBERTYPE_NAMES, TrackNumberType);

    // File Management
    Overwrite = JsonUtils::SafelyNavigate<bool>(json, { "output", "fileManagement", "overwrite" }, Overwrite);
    DefaultSaveLocation = FileUtils::PathFromUtf8(JsonUtils::SafelyNavigate<std::string>(json, { "output", "fileManagement", "defaultSaveLocation" }, FileUtils::PathToUtf8(DefaultSaveLocation)));
    readTaggableInputJson({ "output", "fileManagement", "fileName" }, FileNameTagsOpeningChar, FileNameTagsClosingChar, FileName);
    readTaggableInputJson({ "output", "fileManagement", "subFolders" }, SubFoldersTagsOpeningChar, SubFoldersTagsClosingChar, SubFolders);

    // Lyrics File
    CreateLyricsFile = JsonUtils::SafelyNavigate<bool>(json, { "output", "lyricsFile", "createLyricsFile" }, CreateLyricsFile);
    readTaggableInputJson({ "output", "lyricsFile", "lyricsFileName" }, LyricsFileNameTagsOpeningChar, LyricsFileNameTagsClosingChar, LyricsFileName);

    // Playlist File
    CreatePlaylistFile = JsonUtils::SafelyNavigate<bool>(json, { "output", "playlistFile", "createPlaylistFile" }, CreatePlaylistFile);
    PlaylistFileType = JsonUtils::ReadStringEnum<EPlaylistFileType>(json, { "output", "playlistFile", "playlistFileType" }, EPLAYLISTFILETYPE_NAMES, PlaylistFileType);
    readTaggableInputJson({ "output", "playlistFile", "playlistFileName" }, PlaylistFileNameTagsOpeningChar, PlaylistFileNameTagsClosingChar, PlaylistFileName);

    // == Downloading

    // General
    PerDownloadThreads = JsonUtils::SafelyNavigate<int>(json, { "downloading", "general", "perDownloadThreads" }, PerDownloadThreads);
    DownloadSpeedLimit = JsonUtils::SafelyNavigate<double>(json, { "downloading", "general", "downloadSpeedLimit" }, DownloadSpeedLimit);
    DownloadTimeoutMs = JsonUtils::SafelyNavigate<int>(json, { "downloading", "general", "downloadTimeoutMs" }, DownloadTimeoutMs);
    IPVersion = JsonUtils::ReadStringEnum<EIPVersion>(json, { "downloading", "general", "ipVersion" }, EIPVERSION_NAMES, IPVersion);

    // == Platforms

    // YouTube
    YouTubePOToken = JsonUtils::SafelyNavigate<std::string>(json, { "platforms", "youtube", "poToken" }, YouTubePOToken);

    // == Interface

    // General
    ShowStatusNotifications = JsonUtils::SafelyNavigate<bool>(json, { "interface", "general", "showStatusNotifications" }, ShowStatusNotifications);
    AutoOpenDownloadFolder = JsonUtils::SafelyNavigate<bool>(json, { "interface", "general", "autoOpenDownloadFolder" }, AutoOpenDownloadFolder);

    // Updates
    AutoCheckForUpdates = JsonUtils::SafelyNavigate<bool>(json, { "interface", "updates", "autoCheckForUpdates" }, AutoCheckForUpdates);
    AutoCheckForNotices = JsonUtils::SafelyNavigate<bool>(json, { "interface", "updates", "autoCheckForNotices" }, AutoCheckForNotices);

    // Background
    RandomDefaultBackground = JsonUtils::SafelyNavigate<bool>(json, { "interface", "background", "randomDefaultBackground" }, RandomDefaultBackground);
    DefaultBackgroundUrl = JsonUtils::SafelyNavigate<std::string>(json, { "interface", "background", "defaultBackgroundUrl" }, DefaultBackgroundUrl);
}

bool Config::Load()
{
    return Load(GetFilePath());
}

bool Config::Load(const std::filesystem::path& path)
{
    if (path.empty()) {
        Logger::Warning("Could not load config, the file path was not found");
        return false;
    }

    std::ifstream file(path);
    if (!file.is_open()) {
        Logger::Warning("Could not open config file at " + FileUtils::PathToUtf8(path));

#ifdef _WIN32
        LoadV1Config();
#endif
        Save();

        return false;
    }

    std::string contents = std::string((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
    file.close();

    nlohmann::json json = nlohmann::json::parse(contents);
    FromJson(json);

    Logger::Info("Loaded the config from (" + FileUtils::PathToUtf8(path) + "): " + json.dump());

    return true;
}

bool Config::Save()
{
    return Save(GetFilePath());
}

bool Config::Save(const std::filesystem::path& path)
{
    if (path.empty()) {
        Logger::Warning("Could not save config, the file path was not found");
        return false;
    }

    std::ofstream file(path);
    nlohmann::json json = ToJson();
    std::string contents = json.dump(4);
    file << contents;
    file.close();

    Logger::Info("Saved the config: " + json.dump());

    return true;
}

bool Config::YouTubeCookiesValid()
{
    // TODO: Need to add some way of checking the po token, or checking when updating it in the settings screen
    //       Also should check the age to check if age restricted videos can be accessed

    bool cookiesValid = YouTubeCookieFile::Get().CookiesValid();
    return cookiesValid && !YouTubePOToken.empty();
}

std::filesystem::path Config::GetFilePath()
{
    if (!_filePathCache.empty())
        return _filePathCache;

    std::filesystem::path configPath = Paths::GetConfigDir();
    if (configPath.empty())
        return "";

    _filePathCache = configPath / FILE_NAME;
    return _filePathCache;
}

#ifdef _WIN32
void Config::LoadV1Config()
{
    QSettings settings("WilliamSchack", "Spotify Downloader");

    // Check if v1 was used at all
    if (!settings.allKeys().size() != 0)
        return;
    
    // Output
    settings.beginGroup("Output");
    Overwrite = settings.value("overwriteEnabled", Overwrite).toBool();
    CodecExtension = (EExtension)settings.value("codecIndex", (int)CodecExtension).toInt();
    TrackNumberType = (ETrackNumberType)settings.value("trackNumberIndex", (int)TrackNumberType).toInt();
    Normalise = settings.value("normalizeEnabled", Normalise).toBool();
    NormaliseDb = settings.value("normalizeVolume", NormaliseDb).toFloat();
    GetLyrics = settings.value("getLyrics", GetLyrics).toBool();
    ManualBitrate = !settings.value("automaticBestQuality", !ManualBitrate).toBool();
    BitrateKbps[EExtension::M4A] = settings.value("audioBitrateM4A", BitrateKbps[EExtension::M4A]).toInt();
    BitrateKbps[EExtension::AAC] = settings.value("audioBitrateAAC", BitrateKbps[EExtension::AAC]).toInt();
    BitrateKbps[EExtension::MP3] = settings.value("audioBitrateMP3", BitrateKbps[EExtension::MP3]).toInt();
    BitrateKbps[EExtension::OGG] = settings.value("audioBitrateOGG", BitrateKbps[EExtension::OGG]).toInt();
    DefaultSaveLocation = FileUtils::PathFromUtf8(settings.value("saveLocation", "").toString().toStdString());
    ArtistSeparator = settings.value("artistSeparator", QString::fromStdString(ArtistSeparator)).toString().toStdString();

    std::string fileNameTag = settings.value("songOutputFormatTag", QString(FileNameTagsOpeningChar) + QString(FileNameTagsClosingChar)).toString().toStdString();
    FileNameTagsOpeningChar = fileNameTag[0];
    FileNameTagsClosingChar = fileNameTag[1];
    FileName = settings.value("songOutputFormat", QString::fromStdString(FileName)).toString().toStdString();

    std::string subFoldersTag = settings.value("subFoldersTag", QString(SubFoldersTagsOpeningChar) + QString(SubFoldersTagsClosingChar)).toString().toStdString();
    SubFoldersTagsOpeningChar = subFoldersTag[0];
    SubFoldersTagsClosingChar = subFoldersTag[1];
    SubFolders = settings.value("subFolders", QString::fromStdString(SubFolders)).toString().toStdString();

    CreateLyricsFile = settings.value("lrcFileNameTag", CreateLyricsFile).toBool();
    std::string lyricsFileNameTag = settings.value("lrcFileNameTag", QString(LyricsFileNameTagsOpeningChar) + QString(LyricsFileNameTagsClosingChar)).toString().toStdString();
    LyricsFileNameTagsOpeningChar = lyricsFileNameTag[0];
    LyricsFileNameTagsClosingChar = lyricsFileNameTag[1];
    LyricsFileName = settings.value("lrcFileName", QString::fromStdString(LyricsFileName)).toString().toStdString();

    PlaylistFileType = (EPlaylistFileType)settings.value("playlistFileTypeIndex", (int)PlaylistFileType).toInt();
    CreatePlaylistFile = PlaylistFileType != EPlaylistFileType::None;
    std::string playlistFileNameTag = settings.value("playlistFileNameTag", QString(PlaylistFileNameTagsOpeningChar) + QString(PlaylistFileNameTagsClosingChar)).toString().toStdString();
    PlaylistFileNameTagsOpeningChar = lyricsFileNameTag[0];
    PlaylistFileNameTagsClosingChar = lyricsFileNameTag[1];
    PlaylistFileName = settings.value("playlistFileName", QString::fromStdString(PlaylistFileName)).toString().toStdString();

    settings.endGroup();

    // Downloading
    settings.beginGroup("Downloading");
    ShowStatusNotifications = settings.value("statusNotificationsEnabled", ShowStatusNotifications).toBool();
    PerDownloadThreads = settings.value("downloaderThreads", PerDownloadThreads).toInt();
    DownloadSpeedLimit = settings.value("downloadSpeedLimit", DownloadSpeedLimit).toFloat();
    DownloadTimeoutMs = settings.value("downloadTimeout", DownloadTimeoutMs).toInt();
    IPVersion = (EIPVersion)settings.value("forceIPVersionIndex", (int)IPVersion).toInt();
    settings.endGroup();

    // Interface
    settings.beginGroup("Interface");
    AutoOpenDownloadFolder = settings.value("autoOpenDownloadFolder", AutoOpenDownloadFolder).toBool();
    AutoCheckForUpdates = settings.value("checkForUpdates", AutoCheckForUpdates).toBool();
    AutoCheckForNotices = settings.value("checkForNotices", AutoCheckForNotices).toBool();
    settings.endGroup();

    // Delete the old settings
    settings.clear();

    Logger::Info("Loaded and deleted settings from v1");
}
#endif