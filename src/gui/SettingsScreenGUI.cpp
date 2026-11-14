#include "SettingsScreenGUI.h"

SettingsScreenGUI::SettingsScreenGUI(QObject* parent) : QObject(parent)
{
    connect(this, &SettingsScreenGUI::CodecChanged, this, &SettingsScreenGUI::BitrateMaxChanged);
    connect(this, &SettingsScreenGUI::CodecChanged, this, &SettingsScreenGUI::BitrateMaxPremiumChanged);
    connect(this, &SettingsScreenGUI::CodecChanged, this, &SettingsScreenGUI::BitrateChanged);
    connect(this, &SettingsScreenGUI::CodecChanged, this, &SettingsScreenGUI::UpdateBitrateFromCookieStatus);
    connect(this, &SettingsScreenGUI::CodecChanged, this, &SettingsScreenGUI::LosslessFormatChanged);
    connect(this, &SettingsScreenGUI::CodecChanged, this, &SettingsScreenGUI::FormatIsLosslessChanged);
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
    CleaupYouTubeCookiesWatcher();

    // Save when closing the app
    Config::Save();
}

void SettingsScreenGUI::EnteringScreen()
{
    SetupYouTubeCookiesWatcher();

    // Update settings that can be changed outside of this screen
    emit DefaultSaveLocationChanged();
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

int SettingsScreenGUI::GetBitrateMaxPremium() const
{
    std::unique_ptr<ICodec> codec = CodecFactory::Create(Config::CodecExtension);
    const BitrateDetails& codecBitrateDetails = codec->GetBitrateDetails();
    
    return codecBitrateDetails.MaxBitratePremium;
}

int SettingsScreenGUI::GetLosslessFormat() const
{
    if (!GetFormatIsLossless())
        return 0;

    return (int)Config::LosslessFormats[Config::CodecExtension];
}

void SettingsScreenGUI::SetLosslessFormat(const int& formatIndex)
{
    if (!GetFormatIsLossless())
        return;

    Config::LosslessFormats[Config::CodecExtension] = static_cast<ELosslessCodecFormat>(formatIndex);
}

bool SettingsScreenGUI::GetFormatIsLossless() const
{
    return MapUtils::ContainsKey(Config::LosslessFormats, Config::CodecExtension);
}

void SettingsScreenGUI::DefaultSaveLocationButtonClicked()
{
    QFileDialog dialog(nullptr, "Choose output directory", "");
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOptions(QFileDialog::ShowDirsOnly);
    FileDialogUtils::SetParent(dialog, QGuiApplication::focusWindow());

    if (dialog.exec() != QDialog::Accepted)
        return;
        
    QString folder = dialog.selectedFiles().first();
    if (folder.isEmpty())
        return;

    SetDefaultSaveLocation(folder);
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

bool SettingsScreenGUI::SettingsImportButtonClicked()
{
    QFileDialog dialog(nullptr, "Choose input file", "");
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter("JSON (*.json)");
    FileDialogUtils::SetParent(dialog, QGuiApplication::focusWindow());

    if (dialog.exec() != QDialog::Accepted)
        return false;

    QString file = dialog.selectedFiles().first();
    if (file.isEmpty())
        return false;

    std::filesystem::path path = FileUtils::PathFromUtf8(file.toStdString());
    bool succeeded = Config::Load(path);

    if (succeeded) {
        std::cout << "Successfully loaded config from path: " << path << std::endl;

        // Load the new settings
        NotifyAllSettingsChanged();
    } else
        std::cout << "Failed to load config from path: " << path << std::endl;

    return succeeded;
}

bool SettingsScreenGUI::SettingsExportButtonClicked()
{
    QFileDialog dialog(nullptr, "Choose output file", "");
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter("JSON (*.json)");
    dialog.setDefaultSuffix("json");
    FileDialogUtils::SetParent(dialog, QGuiApplication::focusWindow());

    if (dialog.exec() != QDialog::Accepted)
        return false;

    QString file = dialog.selectedFiles().first();
    if (file.isEmpty())
        return false;

    std::filesystem::path path = FileUtils::PathFromUtf8(file.toStdString());
    bool succeeded = Config::Save(path);

    if (succeeded) {
        std::cout << "Successfully saved config to path: " << path << std::endl;
        
        // Open folder
        QString pathDirectoryString = QString::fromStdString(FileUtils::PathToUtf8(path.parent_path()));
        QDesktopServices::openUrl(QUrl::fromLocalFile(pathDirectoryString));
    } else
        std::cout << "Failed to load save to path: " << path << std::endl;

    return succeeded;
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
    emit BitrateMaxPremiumChanged();
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

void SettingsScreenGUI::NotifyAllSettingsChanged()
{
    const QMetaObject* meta = metaObject();
    for (int i = meta->propertyOffset(); i < meta->propertyCount(); i++) {
        QMetaProperty property = meta->property(i);
        if (property.hasNotifySignal())
            property.notifySignal().invoke(this, Qt::DirectConnection);
    }
}