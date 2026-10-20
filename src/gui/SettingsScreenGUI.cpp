#include "SettingsScreenGUI.h"

SettingsScreenGUI::SettingsScreenGUI(QObject* parent) : QObject(parent)
{
    connect(this, &SettingsScreenGUI::CodecChanged, this, &SettingsScreenGUI::BitrateMaxChanged);
    connect(this, &SettingsScreenGUI::CodecChanged, this, &SettingsScreenGUI::BitrateChanged);
    connect(this, &SettingsScreenGUI::CodecChanged, this, &SettingsScreenGUI::UpdateBitrateFromCookieStatus);
    connect(this, &SettingsScreenGUI::YoutubePOTokenChanged, this, &SettingsScreenGUI::UpdateYouTubeCookiesStatus);
    connect(this, &SettingsScreenGUI::YouTubeCookiesStatusChanged, this, &SettingsScreenGUI::UpdateBitrateFromCookieStatus);

    SetupFileName();
    SetupSubFolders();
    SetupLyricsFileName();
    SetupPlaylistFileName();

    SetupYouTubeCookiesWatcher();
    UpdateYouTubeCookiesStatus();
}

SettingsScreenGUI::~SettingsScreenGUI()
{
    // Save when closing the app
    Config::Save();
}

void SettingsScreenGUI::LeavingScreen()
{
    SaveFileName();
    SaveSubFolders();
    SaveLyricsFileName();
    SavePlaylistFileName();

    CleaupYouTubeCookiesWatcher();

    Config::Save();
}

bool SettingsScreenGUI::GetAllSettingsValid() const
{
    return _fileNameValid && _subFoldersValid && _lyricsFileNameValid && _playlistFileNameValid;
}

int SettingsScreenGUI::GetBitrateMax() const
{
    // TODO: Update tooltip with low, good, high quality bitrate settings

    std::unique_ptr<ICodec> codec = CodecFactory::Create(Config::CodecExtension);
    const BitrateDetails& codecBitrateDetails = codec->GetBitrateDetails();

    bool youtubeCookiesValid = Config::YouTubeCookiesValid();
    
    return youtubeCookiesValid ? codecBitrateDetails.MaxBitratePremium : codecBitrateDetails.MaxBitrate;
}

QString SettingsScreenGUI::GetYouTubeCookiesStatus() const
{
    return _youtubeCookiesStatus;
}

void SettingsScreenGUI::YouTubeCookiesOpenButtonClicked()
{
    CookieFile& cookieFile = YouTubeCookieFile::Get();
    cookieFile.CreateIfEmpty();

    std::filesystem::path filePath = cookieFile.GetPath();
    QString filePathQString = QString::fromStdString(FileUtils::PathToUtf8(filePath));
    QUrl filePathQUrl = QUrl::fromLocalFile(filePathQString);

    QDesktopServices::openUrl(filePathQUrl);
}

void SettingsScreenGUI::YouTubeCookiesPasteButtonClicked()
{
    QClipboard* clipboard = QGuiApplication::clipboard();
    QString clipboardString = clipboard->text();
    YouTubeCookieFile::Get().Overwrite(clipboardString.toStdString());

    UpdateYouTubeCookiesStatus();
}

void SettingsScreenGUI::YouTubeCookiesClearButtonClicked()
{
    YouTubeCookieFile::Get().Clear();

    UpdateYouTubeCookiesStatus();
}

void SettingsScreenGUI::YouTubePOTokenPasteButtonClicked()
{
    QClipboard* clipboard = QGuiApplication::clipboard();
    QString clipboardString = clipboard->text();
    Config::YouTubePOToken = clipboardString.toStdString();

    emit YoutubePOTokenChanged();
}

void SettingsScreenGUI::YouTubePOTokenClearButtonClicked()
{
    Config::YouTubePOToken = "";

    emit YoutubePOTokenChanged();
}

void SettingsScreenGUI::UpdateYouTubeCookiesStatus()
{
    CookieFile& cookieFile = YouTubeCookieFile::Get();
    std::string cookieString = cookieFile.Read();
    bool cookieValid = cookieFile.CookiesValid();

    QString cookieStatus = "";
    if (cookieString.empty())
        cookieStatus = "No Cookies";
    else if (!cookieValid)
        cookieStatus = "Cookies Invalid";

    QString poTokenStatus = "";
    if (Config::YouTubePOToken.empty())
        poTokenStatus = "No PO Token";
    
    if (!cookieStatus.isEmpty() || !poTokenStatus.isEmpty()) {
        _youtubeCookiesStatus = "<font color=\"#ff6464\">Invalid: ";
        _youtubeCookiesStatus += cookieStatus;
        if (!cookieStatus.isEmpty() && !poTokenStatus.isEmpty())
            _youtubeCookiesStatus += ", ";
        _youtubeCookiesStatus += poTokenStatus;
        _youtubeCookiesStatus += "</font>";
    } else {
        _youtubeCookiesStatus = "<font color=\"#18e770\">Cookies Valid</font>";
    }

    emit YouTubeCookiesStatusChanged();
}

void SettingsScreenGUI::UpdateBitrateFromCookieStatus()
{
    bool cookiesValid = Config::YouTubeCookiesValid();

    std::unique_ptr<ICodec> codec = CodecFactory::Create(Config::CodecExtension);
    const BitrateDetails& codecBitrateDetails = codec->GetBitrateDetails();
    int& bitrate = Config::BitrateKbps[Config::CodecExtension];

    if (cookiesValid && bitrate == codecBitrateDetails.MaxBitrate) {
        bitrate = codecBitrateDetails.MaxBitratePremium;
        emit BitrateChanged();
    } else if (!cookiesValid && bitrate > codecBitrateDetails.MaxBitrate) {
        bitrate = codecBitrateDetails.MaxBitrate;
        emit BitrateChanged();
    }

    emit BitrateMaxChanged();
}

void SettingsScreenGUI::SetupYouTubeCookiesWatcher()
{
    // TODO: Do this every time the screen opens, doesnt work on sequential setting screen opens

    connect(&_youtubeCookiesWatcher, &QFileSystemWatcher::fileChanged, this, &SettingsScreenGUI::YouTubeCookiesWatcherFileChanged);
    AddYouTubeCookiesWatcherPath();
}

void SettingsScreenGUI::AddYouTubeCookiesWatcherPath()
{
    std::filesystem::path filePath = YouTubeCookieFile::Get().GetPath();
    QString filePathQString = QString::fromStdString(FileUtils::PathToUtf8(filePath));
    if (!_youtubeCookiesWatcher.files().contains(filePathQString)) {
        if (!_youtubeCookiesWatcher.addPath(filePathQString))
            std::cout << "Failed to watch cookie file at: " << filePath << std::endl;
    }
}

void SettingsScreenGUI::CleaupYouTubeCookiesWatcher()
{
    disconnect(&_youtubeCookiesWatcher, nullptr, this, nullptr);
    _youtubeCookiesWatcher.removePaths(_youtubeCookiesWatcher.files());
}

void SettingsScreenGUI::YouTubeCookiesWatcherFileChanged(const QString& path)
{
    AddYouTubeCookiesWatcherPath();
    YouTubeCookieFile::Get().ClearCache();

    UpdateYouTubeCookiesStatus();
}