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
                { "autoOpenDownloadFolder", AutoOpenDownloadFolder }
            }},
            { "updates", nlohmann::ordered_json::object() } // Will be filled when settings are implemented
        }}
    };

    for (const auto& [key, val] : BitrateKbps) {
        std::string keyString = EEXTENSION_NAMES[(int)key];
        json["output"]["audio"]["bitrate"][keyString] = val;
    }

    return json;
}

void Config::FromJson(const nlohmann::json& json)
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
    CodecExtension = JsonUtils::ReadStringEnum<EExtension>(json, { "output", "audio", "codecExtension" }, EEXTENSION_NAMES, CodecExtension);
    Normalise = JsonUtils::SafelyNavigate<bool>(json, { "output", "audio", "normalise" }, Normalise);
    NormaliseDb = JsonUtils::SafelyNavigate<double>(json, { "output", "audio", "normaliseDb" }, NormaliseDb);
    ManualBitrate = JsonUtils::SafelyNavigate<bool>(json, { "output", "audio", "manualBitrate" }, ManualBitrate);

    nlohmann::json bitrateJson = JsonUtils::SafelyNavigate(json, { "output", "audio", "bitrate" });
    for (const auto& [key, val] : bitrateJson.items()) {
        if (!val.is_number_integer())
            continue;

        int indexOfBitrateExtension = VectorUtils::IndexOf(EEXTENSION_NAMES, key);
        if (indexOfBitrateExtension < 0)
            continue;

        EExtension bitrateExtension = static_cast<EExtension>(indexOfBitrateExtension);
        BitrateKbps[bitrateExtension] = val.get<int>();
    }

    // Metadata
    GetLyrics = JsonUtils::SafelyNavigate<bool>(json, { "output", "metadata", "getLyrics" }, GetLyrics);
    ArtistSeparator = JsonUtils::SafelyNavigate<std::string>(json, { "output", "metadata", "artistSeparator" }, ArtistSeparator);
    TrackNumberType = JsonUtils::ReadStringEnum<ETrackNumberType>(json, { "output", "metadata", "trackNumberType" }, ETRACKNUMBERTYPE_NAMES, TrackNumberType);

    // File Management
    Overwrite = JsonUtils::SafelyNavigate<bool>(json, { "output", "fileManagement", "overwrite" }, Overwrite);
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
    AutoOpenDownloadFolder = JsonUtils::SafelyNavigate<bool>(json, { "interface", "general", "autoOpenDownloadFolder" }, AutoOpenDownloadFolder);

    // Updates
}

void Config::Load()
{
    std::filesystem::path filePath = GetFilePath();
    if (filePath.empty()) {
        std::cout << "Could not load config, the file path was not found" << std::endl;
        return;
    }

    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cout << "Could not open config file at " << filePath << std::endl;

        // TODO: Load old (v1) config if it exists
        Save();

        return;
    }

    std::string contents = std::string((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
    file.close();

    nlohmann::json json = nlohmann::json::parse(contents);
    FromJson(json);
}

void Config::Save()
{
    std::filesystem::path filePath = GetFilePath();
    if (filePath.empty()) {
        std::cout << "Could not save config, the file path was not found" << std::endl;
        return;
    }

    std::ofstream file(filePath);
    std::string contents = ToJson().dump(4);
    file << contents;
    file.close();
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