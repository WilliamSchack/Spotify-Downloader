#include "Config.h"

void Config::Load()
{
    std::filesystem::path filePath = GetFilePath();
    if (filePath.empty()) {
        std::cout << "Could not load config, the file path was not found" << std::endl;
        return;
    }
}

void Config::Save()
{
    std::filesystem::path filePath = GetFilePath();
    if (filePath.empty()) {
        std::cout << "Could not save config, the file path was not found" << std::endl;
        return;
    }
}

bool Config::YouTubeCookiesValid()
{
    // TODO:
    // Need to add some way of checking the po token, or checking when updating it in the settings screen
    // Also should check the age to check if age restricted videos can be accessed

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

void to_json(nlohmann::json& json, const Config& data)
{
    // Store enums as strings incase the order changes later

    json = {
        { "output", {
            { "audio", {
                { "codecExtension", EEXTENSION_NAMES[(int)data.CodecExtension] },
                { "normalise", data.Normalise },
                { "normaliseDb", data.NormaliseDb },
                { "manualBitrate", data.ManualBitrate },
                { "bitrate", nlohmann::json::object() } // Filled later
            }},
            { "metadata", {
                { "getLyrics", data.GetLyrics },
                { "artistSeparator", data.ArtistSeparator },
                { "trackNumberType", ETRACKNUMBERTYPE_NAMES[(int)data.TrackNumberType] }
            }},
            { "fileManagement", {
                { "overwrite", data.Overwrite },
                { "fileName", {
                    { "tagsOpening", std::string(1, data.FileNameTagsOpeningChar) },
                    { "tagsClosing", std::string(1, data.FileNameTagsClosingChar) },
                    { "value", data.FileName }
                }},
                { "subFolders", {
                    { "tagsOpening", std::string(1, data.SubFoldersTagsOpeningChar) },
                    { "tagsClosing", std::string(1, data.SubFoldersTagsClosingChar) },
                    { "value", data.SubFolders }
                }}
            }},
            { "lyricsFile", {
                { "createLyricsFile", data.CreateLyricsFile },
                { "lyricsFileName", {
                    { "tagsOpening", std::string(1, data.LyricsFileNameTagsOpeningChar) },
                    { "tagsClosing", std::string(1, data.LyricsFileNameTagsClosingChar) },
                    { "value", data.LyricsFileName }
                }}
            }},
            { "playlistFile", {
                { "createPlaylistFile", data.CreatePlaylistFile },
                { "playlistFileType", EPLAYLISTFILETYPE_NAMES[(int)data.PlaylistFileType] },
                { "playlistFileName", {
                    { "tagsOpening", std::string(1, data.PlaylistFileNameTagsOpeningChar) },
                    { "tagsClosing", std::string(1, data.PlaylistFileNameTagsClosingChar) },
                    { "value", data.PlaylistFileName }
                }}
            }}
        }},
        { "downloading", {
            { "general", {
                { "perDownloadThreads", data.PerDownloadThreads },
                { "downloadSpeedLimit", data.DownloadSpeedLimit },
                { "downloadTimeoutMs", data.DownloadTimeoutMs },
                { "ipVersion", EIPVERSION_NAMES[(int)data.IPVersion] }
            }}
        }},
        { "platforms", {
            { "youtube", {
                { "poToken", data.YouTubePOToken }
            }}
        }},
        { "interface", {
            { "general", {
                { "autoOpenDownloadFolder", data.AutoOpenDownloadFolder }
            }},
            { "updates", nlohmann::json::object() } // Will be filled when settings are implemented
        }}
    };

    for (const auto& [key, val] : data.BitrateKbps) {
        std::string keyString = EEXTENSION_NAMES[(int)key];
        json["output"]["audio"]["bitrate"][keyString] = val;
    }
}

void from_json(const nlohmann::json& json, Config& data)
{
    // TODO: create taggable input setting type ts is annoying

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
    data.CodecExtension = JsonUtils::ReadStringEnum<EExtension>(json, { "output", "audio", "codecExtension" }, EEXTENSION_NAMES, data.CodecExtension);
    data.Normalise = JsonUtils::SafelyNavigate<bool>(json, { "output", "audio", "normalise" }, data.Normalise);
    data.NormaliseDb = JsonUtils::SafelyNavigate<double>(json, { "output", "audio", "normaliseDb" }, data.NormaliseDb);
    data.ManualBitrate = JsonUtils::SafelyNavigate<bool>(json, { "output", "audio", "manualBitrate" }, data.ManualBitrate);

    nlohmann::json bitrateJson = JsonUtils::SafelyNavigate(json, { "output", "audio", "bitrate" });
    for (const auto& [key, val] : bitrateJson.items()) {
        if (!val.is_number_integer())
            continue;

        int indexOfBitrateExtension = VectorUtils::IndexOf(EEXTENSION_NAMES, key);
        if (indexOfBitrateExtension < 0)
            continue;

        EExtension bitrateExtension = static_cast<EExtension>(indexOfBitrateExtension);
        data.BitrateKbps[bitrateExtension] = val.get<int>();
    }

    // Metadata
    data.GetLyrics = JsonUtils::SafelyNavigate<bool>(json, { "output", "metadata", "getLyrics" }, data.GetLyrics);
    data.ArtistSeparator = JsonUtils::SafelyNavigate<std::string>(json, { "output", "metadata", "artistSeparator" }, data.ArtistSeparator);
    data.TrackNumberType = JsonUtils::ReadStringEnum<ETrackNumberType>(json, { "output", "metadata", "trackNumberType" }, ETRACKNUMBERTYPE_NAMES, data.TrackNumberType);

    // File Management
    data.Overwrite = JsonUtils::SafelyNavigate<bool>(json, { "output", "fileManagement", "overwrite" }, data.Overwrite);
    readTaggableInputJson({ "output", "fileManagement", "fileName" }, data.FileNameTagsOpeningChar, data.FileNameTagsClosingChar, data.FileName);
    readTaggableInputJson({ "output", "fileManagement", "subFolders" }, data.SubFoldersTagsOpeningChar, data.SubFoldersTagsClosingChar, data.SubFolders);

    // Lyrics File
    data.CreateLyricsFile = JsonUtils::SafelyNavigate<bool>(json, { "output", "lyricsFile", "createLyricsFile" }, data.CreateLyricsFile);
    readTaggableInputJson({ "output", "lyricsFile", "lyricsFileName" }, data.LyricsFileNameTagsOpeningChar, data.LyricsFileNameTagsClosingChar, data.LyricsFileName);

    // Playlist File
    data.CreatePlaylistFile = JsonUtils::SafelyNavigate<bool>(json, { "output", "playlistFile", "createPlaylistFile" }, data.CreatePlaylistFile);
    data.PlaylistFileType = JsonUtils::ReadStringEnum<EPlaylistFileType>(json, { "output", "playlistFile", "playlistFileType" }, EPLAYLISTFILETYPE_NAMES, data.PlaylistFileType);
    readTaggableInputJson({ "output", "playlistFile", "playlistFileName" }, data.PlaylistFileNameTagsOpeningChar, data.PlaylistFileNameTagsClosingChar, data.PlaylistFileName);

    // == Downloading

    // General
    data.PerDownloadThreads = JsonUtils::SafelyNavigate<int>(json, { "downloading", "general", "perDownloadThreads" }, data.PerDownloadThreads);
    data.DownloadSpeedLimit = JsonUtils::SafelyNavigate<double>(json, { "downloading", "general", "downloadSpeedLimit" }, data.DownloadSpeedLimit);
    data.DownloadTimeoutMs = JsonUtils::SafelyNavigate<int>(json, { "downloading", "general", "downloadTimeoutMs" }, data.DownloadTimeoutMs);
    data.IPVersion = JsonUtils::ReadStringEnum<EIPVersion>(json, { "downloading", "general", "ipVersion" }, EIPVERSION_NAMES, data.IPVersion);

    // == Platforms

    // YouTube
    data.YouTubePOToken = JsonUtils::SafelyNavigate<std::string>(json, { "platforms", "youtube", "poToken" }, data.YouTubePOToken);

    // == Interface

    // General
    data.AutoOpenDownloadFolder = JsonUtils::SafelyNavigate<bool>(json, { "interface", "general", "autoOpenDownloadFolder" }, data.AutoOpenDownloadFolder);

    // Updates
}