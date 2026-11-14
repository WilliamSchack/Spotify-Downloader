import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts

RowLayout {
    id: root
    spacing: 10

    property real scrollAnimationTime: 200

    property string validTrackTagsString: "**Song Name** - Name of the song<br />**Album Name** - Name of the album<br />**Song Artist** - Name of the first artist in the song<br />**Song Artists** - Name of all the artists in a song separated by \",\"<br />**Album Artist** - Name of the first artist in the songs album<br />**Album Artists** - me of all the artists in a songs album separated by \",\"<br />**Codec** - The output codec<br />**Track Number** - The track number as set in the Track Number setting<br />**Playlist Track Number** - The track number in the playlist, or the album if a playlist is not inputted<br />**Album Track Number** - The track number in the album<br />**Disc Number** - The track number in the disc<br />**Song Time Seconds** - The duration of the song in seconds<br />**Song Time Minutes** - The duration of the song in minutes \"01.23\"<br />**Song Time Hours** - The duration of the song in hours \"00.01.23\"<br />**Year** - The release year of the song<br />**Month** - The release month of the song<br />**Day** - The release day of the song"
    property string validSaveableTrackTagsString: "**Save Location** - The output path set in the Add Tracks popup<br />" + validTrackTagsString
    property string validPlaylistTagsString: "**Playlist Name** - Name of the downloaded playlist, album, or track depending on what was inputted<br />**Playlist Owner** - Name of the playlist owner, album main artist, or track main artist"
    property string validSaveablePlaylistTagsString: "**Save Location** - The output path set in the Add Tracks popup<br />" + validPlaylistTagsString

    // Left Area
    Rectangle {
        Layout.fillHeight: true
        Layout.preferredWidth: 260

        radius: 10
        gradient: Gradient {
            GradientStop { position: 0.0; color: Qt.alpha("#373737", 0.8) }
            GradientStop { position: 1.0; color: Qt.alpha("#3D2B3F", 0.8) }
        }

        Item {
            id: sidebarRoot
            anchors.fill: parent
            anchors.margins: 20

            property bool trackScrolling: true

            property SettingsSidebarLink activeLink: null
            property list<SettingsSidebarGroup> groups: [sidebarGroupOutput, sidebarGroupDownloading, sidebarGroupPlatforms, sidebarGroupInterface]
            property int activeGroupIndex: 0

            function updateLinkFromScroll() {
                var scrollY = settingsFlickableRight.contentY
                var bestMatch = { group: groups[0], link: groups[0].linkItems[0] }

                for (var i = 0; i < groups.length; i++) {
                    var group = groups[i]
                    for (var j = 0; j < group.linkItems.length; j++) {
                        var linkItem = group.linkItems[j]
                        var sectionY = linkItem.target.mapToItem(settingsColumnRight, 0, 0).y
                        if (sectionY <= scrollY) {
                            bestMatch.group = group
                            bestMatch.link = linkItem
                        }
                    }
                }

                if (bestMatch.link !== activeLink)
                    setActiveLink(bestMatch.link, bestMatch.group, false)
            }

            function setActiveLink(link, group, preventScrollTracking) {
                activeLink = link
                var targetY = link.mapToItem(group.track, 0, 0).y

                // Prevent scrolling changing the active link while this one is animating
                if (preventScrollTracking) {
                    sidebarRoot.trackScrolling = false
                    scrollTrackResetTimer.restart()
                }

                var newGroupIndex = groups.indexOf(group)
                if (newGroupIndex === activeGroupIndex) {
                    group.track.indicator.y = targetY
                    return
                }

                // Move the old indicator off the item up or down depending on location of the next track
                // Do the same but opposite for the next track

                var movingDown = newGroupIndex > activeGroupIndex
                var oldTrack = groups[activeGroupIndex].track
                var newTrack = groups[newGroupIndex].track

                oldTrack.indicator.y = movingDown ? oldTrack.height : -oldTrack.indicator.height
                newTrack.snapToY(movingDown ? -newTrack.indicator.height : newTrack.height)

                activeGroupIndex = newGroupIndex

                // Offset the next indicator so the first one has time to move off the original track
                relayTimer.targetY = targetY
                relayTimer.targetTrack = newTrack
                relayTimer.start()
            }

            Timer {
                id: relayTimer
                interval: root.scrollAnimationTime / 4
                property real targetY: 0
                property SettingsSidebarTrack targetTrack: null
                onTriggered: targetTrack.indicator.y = targetY
            }

            Timer {
                id: scrollTrackResetTimer
                interval: root.scrollAnimationTime
                onTriggered: sidebarRoot.trackScrolling = true
            }

            Connections {
                target: settingsFlickableRight
                function onContentYChanged() {
                    if (sidebarRoot.trackScrolling)
                        sidebarRoot.updateLinkFromScroll()
                }
            }

            Flickable {
                id: settingsFlickableLeft
                anchors.fill: parent
                clip: true

                contentWidth: width
                contentHeight: settingsColumnLeft.height
                
                property bool scrollBarActive: contentHeight > height

                ScrollBar.vertical: ScrollBar {
                    policy: settingsFlickableLeft.scrollBarActive ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
                }

                Column {
                    id: settingsColumnLeft
                    width: parent.width - (settingsFlickableLeft.scrollBarActive ? 20 : 0)
                    spacing: 20

                    SettingsSidebarGroup {
                        id: sidebarGroupOutput
                        label: "Output"
                        animationTime: root.scrollAnimationTime
                        flickable: settingsFlickableRight
                        activeLink: sidebarRoot.activeLink
                        onActivated: (link, group, clicked) => { sidebarRoot.setActiveLink(link, group, clicked) }

                        SettingsSidebarLink {
                            label: "Audio"
                            target: sectionAudio
                            activeByDefault: true
                        }

                        SettingsSidebarLink {
                            label: "Metadata"
                            target: sectionMetadata
                        }

                        SettingsSidebarLink {
                            label: "File Management"
                            target: sectionFileManagement
                        }

                        SettingsSidebarLink {
                            label: "Lyrics File"
                            target: sectionLyricsFile
                        }

                        SettingsSidebarLink {
                            label: "Playlist File"
                            target: sectionPlaylistFile
                        }
                    }

                    SettingsSidebarGroup {
                        id: sidebarGroupDownloading
                        label: "Downloading"
                        animationTime: root.scrollAnimationTime
                        flickable: settingsFlickableRight
                        activeLink: sidebarRoot.activeLink
                        onActivated: (link, group, clicked) => { sidebarRoot.setActiveLink(link, group, clicked) }

                        SettingsSidebarLink {
                            label: "General"
                            target: sectionDownloadingGeneral
                        }
                    }

                    SettingsSidebarGroup {
                        id: sidebarGroupPlatforms
                        label: "Platforms"
                        animationTime: root.scrollAnimationTime
                        flickable: settingsFlickableRight
                        activeLink: sidebarRoot.activeLink
                        onActivated: (link, group, clicked) => { sidebarRoot.setActiveLink(link, group, clicked) }

                        SettingsSidebarLink {
                            label: "YouTube"
                            target: sectionYouTube
                        }
                    }

                    SettingsSidebarGroup {
                        id: sidebarGroupInterface
                        label: "Interface"
                        animationTime: root.scrollAnimationTime
                        flickable: settingsFlickableRight
                        activeLink: sidebarRoot.activeLink
                        onActivated: (link, group, clicked) => { sidebarRoot.setActiveLink(link, group, clicked) }

                        SettingsSidebarLink {
                            label: "General"
                            target: sectionInterfaceGeneral
                        }

                        SettingsSidebarLink {
                            label: "Updates"
                            target: sectionUpdates
                        }

                        SettingsSidebarLink {
                            label: "Background"
                            target: sectionBackground
                        }

                        SettingsSidebarLink {
                            label: "Settings"
                            target: sectionSettings
                        }
                    }
                }
            }
        }
    }

    // Right Area
    Rectangle {
        Layout.fillHeight: true
        Layout.fillWidth: true

        radius: 10
        gradient: Gradient {
            GradientStop { position: 0.0; color: Qt.alpha("#373737", 0.8) }
            GradientStop { position: 1.0; color: Qt.alpha("#3D2B3F", 0.8) }
        }
        
        // This item is just for the margin
        Item {
            anchors.fill: parent
            anchors.margins: 20

            // Using Flickable for mouse drag
            Flickable {
                id: settingsFlickableRight
                anchors.fill: parent
                clip: true

                contentWidth: width
                contentHeight: settingsColumnRight.height

                property bool scrollBarActive: contentHeight > height

                Behavior on contentY {
                    NumberAnimation {
                        duration: root.scrollAnimationTime
                        easing.type: Easing.OutCubic
                    }
                }

                ScrollBar.vertical: ScrollBar {
                    policy: settingsFlickableRight.scrollBarActive ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
                }

                Column {
                    id: settingsColumnRight
                    width: parent.width - (settingsFlickableRight.scrollBarActive ? 20 : 0)
                    spacing: 30

                    MajorSection {
                        label: "Output"

                        MinorSection {
                            id: sectionAudio
                            label: "Audio"

                            SettingsItem {
                                label: "Codec"
                                tooltip: "This will be converted from what the searched platform provides<br />For example YouTube provides m4a and webm files, the file will be converted from the m4a or webm to this codec\n\n**This also means that codecs that should be lossless (eg. wav, flac) are lossy as a result**"

                                CustomComboBox {
                                    model: _manager.settings.GetCodecOptions()
                                    currentIndex: _manager.settings.codec
                                    onActivated: (index) => { _manager.settings.codec = index }
                                }
                            }

                            SettingsItem {
                                label: "Normalise Volume"
                                tooltip: "Increases/Decreases the volume of the output based on the average over the whole track\n\n**Loud:** -11dB<br />**Normal:** -14dB<br />**Quiet:** -17dB"

                                CustomSwitch {
                                    checked: _manager.settings.normalise
                                    onClicked: _manager.settings.normalise = checked
                                }

                                DoubleTextField {
                                    enabled: _manager.settings.normalise
                                    min: -50.0
                                    max: 50.0
                                    decimals: 1
                                    rightText: "dB"
                                    hasRightLabel: true

                                    boundValue: _manager.settings.normaliseDb
                                    onValueChanged: (value) => { _manager.settings.normaliseDb = value }
                                }
                            }

                            SettingsItem {
                                visible: !_manager.settings.formatIsLossless
                                label: "Audio Bitrate"
                                tooltip: "**Best Quality:** Doesn't modify the bitrate from the downloaded file<br />**Manual:** Converts the bitrate to the value set on the right\n\nOnly accepts numbers within 33-" + _manager.settings.bitrateMax + " with a multiple of 32\n\nIf you are using YouTube Premium, this value can be increased to " + _manager.settings.bitrateMaxPremium + "kbps"

                                CustomComboBox {
                                    width: 150

                                    model: ["Best Quality", "Manual"]

                                    currentIndex: _manager.settings.manualBitrate ? 1 : 0
                                    onActivated: (index) => { _manager.settings.manualBitrate = index == 1 }
                                }

                                IntTextField {
                                    // Should change based on codec, premium status, and auto update when manualBitrate (Same as v1)
                                    enabled: _manager.settings.manualBitrate
                                    min: 33
                                    max: _manager.settings.bitrateMax
                                    rightText: "kb/s"
                                    hasRightLabel: true

                                    extraValidation: (value) => {
                                        // Snap to multiple of 32
                                        if (value % 32 == 0)
                                            return value

                                        var remainder = value % 32
                                        if (remainder < 16)
                                            value -= remainder
                                        else
                                            value += 32 - remainder

                                        return value
                                    }

                                    boundValue: _manager.settings.bitrate
                                    onValueChanged: (value) => { _manager.settings.bitrate = value }
                                }
                            }

                            SettingsItem {
                                visible: _manager.settings.formatIsLossless
                                label: "Audio Format"
                                tooltip: "**Only change this if you really need another format**\n\nAll downloads will be lossy so this really just pads out the file"

                                CustomComboBox {
                                    model: _manager.settings.GetLosslessCodecFormatOptions()

                                    currentIndex: _manager.settings.losslessFormat
                                    onActivated: (index) => { _manager.settings.losslessFormat = index }
                                }
                            }
                        }

                        MinorSection {
                            id: sectionMetadata
                            label: "Metadata"

                            SettingsItem {
                                label: "Get Lyrics"
                                tooltip: "Fetches the lyrics and puts it into the metadata"

                                CustomSwitch {
                                    checked: _manager.settings.getLyrics
                                    onClicked: _manager.settings.getLyrics = checked
                                }
                            }

                            SettingsItem {
                                label: "Artist Separator"
                                tooltip: "Sets the separator between artists in the metadata\n\n**\"; \" is recommended as it has the most compatibility**"

                                CustomTextField {
                                    text: _manager.settings.artistsSeparator
                                    onTextEdited: _manager.settings.artistsSepanrator = text
                                }
                            }

                            SettingsItem {
                                label: "Track Number"
                                tooltip: "**Playlist:** Uses the number in the playlist, or the album if a playlist is not inputted<br />**Album:** Uses the number in the album<br />**Disc:** Uses the number in the disc"

                                // TODO: Add notice/warning when using playlist track number
                                //       that it will use the album number when not downloading a playlist

                                CustomComboBox {
                                    model: _manager.settings.GetTrackNumberTypeOptions()
                                    currentIndex: _manager.settings.trackNumberType
                                    onActivated: (index) => { _manager.settings.trackNumberType = index }
                                }
                            }
                        }

                        MinorSection {
                            id: sectionFileManagement
                            label: "File Management"

                            SettingsItem {
                                label: "Overwrite"
                                tooltip: "Overwrites existing files only if they have the same metadata"

                                CustomSwitch {
                                    checked: _manager.settings.overwrite
                                    onClicked: _manager.settings.overwrite = checked
                                }
                            }

                            SettingsItem {
                                label: "Default Save Location"
                                tooltip: "The folder that is inputted into the Add Tracks popup by default"

                                CustomTextField {
                                    // Increasing width to fit with below inputs
                                    width: 310 + height

                                    text: _manager.settings.defaultSaveLocation
                                    onTextEdited: _manager.settings.defaultSaveLocation = text
                                }

                                CustomButton {
                                    width: parent.height
                                    height: parent.height

                                    onClicked: _manager.settings.DefaultSaveLocationButtonClicked()
                                }
                            }

                            SettingsItem {
                                label: "File Name"
                                tooltip: "Specifies how the file will be named<br />If there are files with the same name, the tracks id will be appended to the end\n\nUses the tag start and end for the bounds of tags:<br />eg. \"&lt;&gt;\": \"&lt;Song Name&gt; - &lt;Song Artist&gt;\"\n\n**Valid Tags**<br />" + root.validTrackTagsString
                                error: _manager.settings.fileNameError
                                
                                SingleCharTextField {
                                    text: _manager.settings.fileNameTagsOpeningChar
                                    onTextEdited: _manager.settings.fileNameTagsOpeningChar = text
                                }

                                SingleCharTextField {
                                    text: _manager.settings.fileNameTagsClosingChar
                                    onTextEdited: _manager.settings.fileNameTagsClosingChar = text
                                }

                                CustomTextField {
                                    text: _manager.settings.fileName
                                    onTextEdited: _manager.settings.fileName = text

                                    valid: _manager.settings.fileNameError === ""
                                }
                            }

                            SettingsItem {
                                label: "Sub Folders"
                                tooltip: "Creates folders within the set download path with the layout set here\n\neg. If you want to output to DownloadPath/SongArtist/AlbumName<br />\"&lt;&gt;\": \"&lt;Song Artist&gt;/&lt;Album Name&gt;\"\n\n**Valid Tags**<br />" + root.validTrackTagsString
                                error: _manager.settings.subFoldersError

                                SingleCharTextField {
                                    text: _manager.settings.subFoldersTagsOpeningChar
                                    onTextEdited: _manager.settings.subFoldersTagsOpeningChar = text
                                }

                                SingleCharTextField {
                                    text: _manager.settings.subFoldersTagsClosingChar
                                    onTextEdited: _manager.settings.subFoldersTagsClosingChar = text
                                }

                                CustomTextField {
                                    text: _manager.settings.subFolders
                                    onTextEdited: _manager.settings.subFolders = text

                                    valid: _manager.settings.subFoldersError === ""
                                }
                            }
                        }

                        MinorSection {
                            id: sectionLyricsFile
                            label: "Lyrics File"

                            SettingsItem {
                                label: "Auto Create On Download"
                                tooltip: "Creates a .lrc file for each track at the set LRC File path"

                                CustomSwitch {
                                    checked: _manager.settings.createLyricsFile
                                    onClicked: _manager.settings.createLyricsFile = checked
                                }
                            }

                            SettingsItem {
                                label: "LRC File Path"
                                tooltip: "Specifies how the lrc file will be named<br />This must be an **absolute path**. You can use &lt;Save Location&gt; to make it relative\n\neg. A song named \"Song\" by \"Artist\" downloaded at \"C:/Songs\"<br />**Format:** \"&lt;&gt;\": \"&lt;Save Location&gt;/&lt;Song Name&gt; - &lt;Song Artist&gt;\"<br />**Output:** \"C:/Songs/Song - Artist.lrc\n\n**Valid Tags**<br />" + root.validSaveableTrackTagsString
                                error: _manager.settings.lyricsFileNameError

                                SingleCharTextField {
                                    text: _manager.settings.lyricsFileNameTagsOpeningChar
                                    onTextEdited: _manager.settings.lyricsFileNameTagsOpeningChar = text
                                }

                                SingleCharTextField {
                                    text: _manager.settings.lyricsFileNameTagsClosingChar
                                    onTextEdited: _manager.settings.lyricsFileNameTagsClosingChar = text
                                }

                                CustomTextField {
                                    text: _manager.settings.lyricsFileName
                                    onTextEdited: _manager.settings.lyricsFileName = text

                                    valid: _manager.settings.lyricsFileNameError === ""
                                }
                            }
                        }

                        MinorSection {
                            id: sectionPlaylistFile
                            label: "Playlist File"

                            SettingsItem {
                                label: "Auto Create On Download"
                                tooltip: "Creates a playlist file when a download finishes containing all the downloaded tracks"

                                CustomSwitch {
                                    checked: _manager.settings.createPlaylistFile
                                    onClicked: _manager.settings.createPlaylistFile = checked
                                }
                            }

                            SettingsItem {
                                label: "Playlist File Type"
                                tooltip: "The type of playlist file to save"

                                CustomComboBox {
                                    model: _manager.settings.GetPlaylistFileTypeOptions()
                                    currentIndex: _manager.settings.playlistFileType
                                    onActivated: (index) => { _manager.settings.playlistFileType = index }
                                }
                            }

                            SettingsItem {
                                label: "Playlist File Path"
                                tooltip: "Specifies how the playlist file will be named<br />This must be an **absolute path**. You can use &lt;Save Location&gt; to make it relative\n\neg. An m3u playlist named \"Playlist\" by \"Owner\" downloaded at \"C:/Songs\"<br />**Format:** \"&lt;&gt;\": \"&lt;Save Location&gt;/&lt;Playlist Name&gt; - &lt;Playlist Owner&gt;\"<br />**Output:** \"C:/Songs/Playlist - Owner.m3u\n\n**Valid Tags**<br />" + root.validSaveablePlaylistTagsString
                                error: _manager.settings.playlistFileNameError

                                SingleCharTextField {
                                    text: _manager.settings.playlistFileNameTagsOpeningChar
                                    onTextEdited: _manager.settings.playlistFileNameTagsOpeningChar = text
                                }

                                SingleCharTextField {
                                    text: _manager.settings.playlistFileNameTagsClosingChar
                                    onTextEdited: _manager.settings.playlistFileNameTagsClosingChar = text
                                }

                                CustomTextField {
                                    text: _manager.settings.playlistFileName
                                    onTextEdited: _manager.settings.playlistFileName = text

                                    valid: _manager.settings.playlistFileNameError === ""
                                }
                            }
                        }
                    }

                    MajorSection {
                        label: "Downloading"

                        MinorSection {
                            id: sectionDownloadingGeneral
                            label: "General"

                            SettingsItem {
                                label: "Max Simultaneous Downloads"
                                tooltip: "How many tracks can be downloaded at the same time<br />Does not change in progress downloads\n\nDont use a high number as it can increase CPU usage<br />**6 is recommended** as its a good middle ground"

                                IntTextField {
                                    min: 0
                                    max: 128

                                    boundValue: _manager.settings.downloadThreads
                                    onValueChanged: (value) => { _manager.settings.downloadThreads = value }
                                }
                            }

                            SettingsItem {
                                label: "Download Speed Limit"
                                tooltip: "The speed limit of the download stage\n\n0 is uncapped"

                                DoubleTextField {
                                    min: 0.0
                                    max: 999999999.9
                                    rightText: "MB/s"
                                    hasRightLabel: true

                                    boundValue: _manager.settings.downloadSpeedLimit
                                    onValueChanged: (value) => { _manager.settings.downloadSpeedLimit = value }
                                }
                            }

                            SettingsItem {
                                label: "Download Timeout"
                                tooltip: "The amount of time before the download stage cancels and fails the download\n\nUsed incase when a download gets stuck for any reason"

                                IntTextField {
                                    min: 5000
                                    max: 999999999
                                    rightText: "ms"
                                    hasRightLabel: true

                                    boundValue: _manager.settings.downloadTimeoutMs
                                    onValueChanged: (value) => { _manager.settings.downloadTimeoutMs = value }
                                }
                            }

                            SettingsItem {
                                label: "IP Version"
                                tooltip: "The IP Version that will be used for downloading\n\nSet this to one of the values if all downloads fail with a timeout error"

                                CustomComboBox {
                                    model: _manager.settings.GetIPVersionOptions()
                                    currentIndex: _manager.settings.ipVersion
                                    onActivated: (index) => { _manager.settings.ipVersion = index }
                                }
                            }
                        }
                    }

                    MajorSection {
                        label: "Platforms"

                        MinorSection {
                            id: sectionYouTube
                            label: "YouTube"

                            SettingsItem {
                                label: "Status"

                                Text {
                                    // Fill to next settings: input + 2 buttons + 2 spaces
                                    width: 300 + 2 * height + 10 * 2

                                    text: _manager.settings.youtubeCookiesStatus
                                    textFormat: Text.RichText
                                    font.pixelSize: 16
                                    font.bold: true
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }

                            SettingsItem {
                                label: "Cookies"
                                tooltip: "The YouTube cookies in the netscape format<br />The cookies must be inputted into the file that opens when you click \"Open File\"\n\nView the readme for instructions on how to get this"

                                CustomButton {
                                    width: 300
                                    height: parent.height

                                    text: "Open File"
                                    textColour: "black"
                                    font.bold: true

                                    onClicked: _manager.settings.YouTubeCookiesOpenButtonClicked();
                                }

                                // Paste
                                CustomButton {
                                    width: parent.height
                                    height: parent.height

                                    onClicked: _manager.settings.YouTubeCookiesPasteButtonClicked()
                                }

                                // Clear
                                CustomButton {
                                    width: parent.height
                                    height: parent.height

                                    onClicked: _manager.settings.YouTubeCookiesClearButtonClicked()
                                }
                            }

                            SettingsItem {
                                label: "PO Token"
                                tooltip: "The PO token used along side the cookies\n\nView the readme for instructions on how to get this"

                                CustomTextField {
                                    text: _manager.settings.youtubePOToken
                                    onTextEdited: _manager.settings.youtubePOToken = text
                                }

                                // Paste
                                CustomButton {
                                    width: parent.height
                                    height: parent.height

                                    onClicked: _manager.settings.YouTubePOTokenPasteButtonClicked()
                                }

                                // Clear
                                CustomButton {
                                    width: parent.height
                                    height: parent.height

                                    onClicked: _manager.settings.YouTubePOTokenClearButtonClicked()
                                }
                            }
                        }
                    }

                    MajorSection {
                        label: "Interface"

                        MinorSection {
                            id: sectionInterfaceGeneral
                            label: "General"

                            SettingsItem {
                                label: "Show Status Notifications"
                                tooltip: "Shows status updates via system notifications"

                                CustomSwitch {
                                    checked: _manager.settings.showStatusNotifications
                                    onClicked: _manager.settings.showStatusNotifications = checked
                                }
                            }

                            SettingsItem {
                                label: "Auto Open Download Folder"
                                tooltip: "Automatically opens the output folder when a download finishes"

                                CustomSwitch {
                                    checked: _manager.settings.autoOpenDownloadFolder
                                    onClicked: _manager.settings.autoOpenDownloadFolder = checked
                                }
                            }
                        }

                        MinorSection {
                            id: sectionUpdates
                            label: "Updates"

                            SettingsItem {
                                label: "Check For Updates"
                                tooltip: "Checks for dependency updates when opening the app"

                                CustomSwitch {
                                    checked: _manager.settings.autoCheckForUpdates
                                    onClicked: _manager.settings.autoCheckForUpdates = checked
                                }
                            }

                            SettingsItem {
                                label: "Check For Notices"
                                tooltip: "Checks for notices when opening the app"

                                CustomSwitch {
                                    checked: _manager.settings.autoCheckForNotices
                                    onClicked: _manager.settings.autoCheckForNotices = checked
                                }
                            }
                        }

                        MinorSection {
                            id: sectionBackground
                            label: "Background"

                            SettingsItem {
                                label: "Random Default Background"
                                tooltip: "Randomly assigns a background based on a preset list of albums"

                                CustomSwitch {
                                    checked: _manager.settings.randomDefaultBackground
                                    onClicked: _manager.settings.randomDefaultBackground = checked
                                }
                            }

                            SettingsItem {
                                label: "Default Background Url"
                                tooltip: "The url used to get the background when opening the app before a download<br />This will not happen when Random Default Background is enabled\n\nClicking apply will overwrite the current background with this one"

                                CustomTextField {
                                    text: _manager.settings.defaultBackgroundUrl
                                    onTextEdited: _manager.settings.defaultBackgroundUrl = text
                                }

                                CustomButton {
                                    width: parent.height * 2 + 10
                                    height: parent.height

                                    enabled: !_manager.music.loadingBackgroundImage

                                    text: _manager.music.loadingBackgroundImage ? "Loading" : "Apply"
                                    textColour: "black"
                                    font.bold: true

                                    onClicked: {
                                        _manager.settings.randomDefaultBackground = false
                                        _manager.music.BackgroundApplyButtonClicked()
                                    }
                                }
                            }
                        }

                        // TODO: Add popup for when failed / succeeded
                        MinorSection {
                            id: sectionSettings
                            label: "Settings"

                            SettingsItem {
                                label: "Import"
                                tooltip: "Sets the settings from a selected .json file<br />This will overwrite your settings\n\nIt is assumed this file is in the correct format<br />You can view the format by exporting the settings below"

                                CustomButton {
                                    width: 300
                                    height: parent.height

                                    text: "Import From File"
                                    textColour: "black"
                                    font.bold: true

                                    onClicked: _manager.settings.SettingsImportButtonClicked()
                                }
                            }

                            SettingsItem {
                                label: "Export"
                                tooltip: "Exports the current settings to a .json file"

                                CustomButton {
                                    width: 300
                                    height: parent.height

                                    text: "Export To File"
                                    textColour: "black"
                                    font.bold: true

                                    onClicked: _manager.settings.SettingsExportButtonClicked()
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}