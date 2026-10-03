import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

RowLayout {
    spacing: 10

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

            property real indicatorAnimationTime: 200

            property SettingsSidebarLink activeLink: null
            property list<SettingsSidebarGroup> groups: [sidebarGroupOutput, sidebarGroupDownloading, sidebarGroupPlatforms, sidebarGroupInterface]
            property int activeGroupIndex: 0

            function setActiveLink(link, group) {
                activeLink = link
                var targetY = link.mapToItem(group.track, 0, 0).y

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
                interval: sidebarRoot.indicatorAnimationTime / 4
                property real targetY: 0
                property SettingsSidebarTrack targetTrack: null
                onTriggered: targetTrack.indicator.y = targetY
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
                        animationTime: sidebarRoot.indicatorAnimationTime
                        flickable: settingsFlickableRight
                        activeLink: sidebarRoot.activeLink
                        onActivated: (link, group) => { sidebarRoot.setActiveLink(link, group) }

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
                    }

                    SettingsSidebarGroup {
                        id: sidebarGroupDownloading
                        label: "Downloading"
                        animationTime: sidebarRoot.indicatorAnimationTime
                        flickable: settingsFlickableRight
                        activeLink: sidebarRoot.activeLink
                        onActivated: (link, group) => { sidebarRoot.setActiveLink(link, group) }

                        SettingsSidebarLink {
                            label: "General"
                            target: sectionDownloadingGeneral
                        }
                    }

                    SettingsSidebarGroup {
                        id: sidebarGroupPlatforms
                        label: "Platforms"
                        animationTime: sidebarRoot.indicatorAnimationTime
                        flickable: settingsFlickableRight
                        activeLink: sidebarRoot.activeLink
                        onActivated: (link, group) => { sidebarRoot.setActiveLink(link, group) }

                        SettingsSidebarLink {
                            label: "YouTube"
                            target: sectionYouTube
                        }
                    }

                    SettingsSidebarGroup {
                        id: sidebarGroupInterface
                        label: "Interface"
                        animationTime: sidebarRoot.indicatorAnimationTime
                        flickable: settingsFlickableRight
                        activeLink: sidebarRoot.activeLink
                        onActivated: (link, group) => { sidebarRoot.setActiveLink(link, group) }

                        SettingsSidebarLink {
                            label: "General"
                            target: sectionInterfaceGeneral
                        }

                        SettingsSidebarLink {
                            label: "Updates"
                            target: sectionUpdates
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
                        duration: 200
                        easing.type: Easing.OutCubic
                    }
                }

                ScrollBar.vertical: ScrollBar {
                    policy: settingsFlickableRight.scrollBarActive ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
                }

                Column {
                    id: settingsColumnRight
                    width: parent.width - (settingsFlickableRight.scrollBarActive ? 20 : 0)
                    spacing: 20

                    SettingsMajorSection {
                        label: "Output"

                        SettingsMinorSection {
                            id: sectionAudio
                            label: "Audio"

                            SettingsItem {
                                label: "Codec"

                                CustomComboBox {
                                    model: _manager.settings.GetCodecOptions()
                                    currentIndex: _manager.settings.codec
                                    onActivated: (index) => { _manager.settings.codec = index }
                                }
                            }

                            SettingsItem {
                                label: "Normalise Volume"

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

                                    text: _manager.settings.normaliseDb
                                    onTextEdited: _manager.settings.normaliseDb = parseFloat(text)
                                }
                            }

                            SettingsItem {
                                label: "Audio Bitrate"

                                CustomComboBox {
                                    width: 150

                                    model: ["Best Quality", "Manual"]

                                    currentIndex: _manager.settings.manualBitrate ? 1 : 0
                                    onActivated: (index) => { _manager.settings.manualBitrate = index == 1 }
                                }

                                IntTextField {
                                    // Should change based on codec, premium status, and auto update when manualBitrate (Same as v1)
                                    enabled: _manager.settings.manualBitrate
                                    min: 0
                                    max: 128
                                    rightText: "kb/s"
                                    hasRightLabel: true

                                    text: _manager.settings.bitrate
                                    onTextEdited: (index) => { _manager.settings.bitrate = parseInt(text) }
                                }
                            }
                        }

                        SettingsMinorSection {
                            id: sectionMetadata
                            label: "Metadata"

                            SettingsItem {
                                label: "Get Lyrics"

                                CustomSwitch {
                                    checked: _manager.settings.getLyrics
                                    onClicked: _manager.settings.getLyrics = checked
                                }
                            }

                            SettingsItem {
                                label: "Artist Separator"

                                CustomTextField {
                                    text: _manager.settings.artistsSeperator
                                    onTextEdited: _manager.settings.artistsSeperator = text
                                }
                            }

                            SettingsItem {
                                label: "Track Number"

                                CustomComboBox {
                                    model: ["Playlist", "Album"/*, ...*/]
                                }
                            }
                        }

                        SettingsMinorSection {
                            id: sectionFileManagement
                            label: "File Management"

                            SettingsItem {
                                label: "Overwrite"

                                CustomSwitch {
                                    checked: _manager.settings.overwrite
                                    onClicked: _manager.settings.overwrite = checked
                                }
                            }

                            SettingsItem {
                                label: "File Name"

                                SingleCharTextField {}
                                SingleCharTextField {}
                                CustomTextField {}
                            }

                            SettingsItem {
                                label: "Sub Folders"

                                SingleCharTextField {}
                                SingleCharTextField {}
                                CustomTextField {}
                            }
                        }

                        SettingsMinorSection {
                            id: sectionLyricsFile
                            label: "Lyrics File"

                            SettingsItem {
                                label: "Auto Create On Download"

                                CustomSwitch {}
                            }

                            SettingsItem {
                                label: "LRC File Name"

                                SingleCharTextField {}
                                SingleCharTextField {}
                                CustomTextField {}
                            }
                        }

                        SettingsMinorSection {
                            id: sectionPlaylistFile
                            label: "Playlist File"

                            SettingsItem {
                                label: "Auto Create On Download"

                                CustomSwitch {}
                            }

                            SettingsItem {
                                label: "Playlist File Type"

                                CustomComboBox {
                                    model: ["M3U", "XSPF"/*, ...*/]
                                }
                            }

                            SettingsItem {
                                label: "Playlist File Name"

                                SingleCharTextField {}
                                SingleCharTextField {}
                                CustomTextField {}
                            }
                        }
                    }

                    SettingsMajorSection {
                        label: "Downloading"

                        SettingsMinorSection {
                            id: sectionDownloadingGeneral
                            label: "General"

                            SettingsItem {
                                label: "Max Simultaneous Downloads"

                                IntTextField {
                                    min: 0
                                    max: 128

                                    text: _manager.settings.downloadThreads
                                    onTextEdited: _manager.settings.downloadThreads = text
                                }
                            }

                            SettingsItem {
                                label: "Download Speed Limit"

                                DoubleTextField {
                                    min: 0.0
                                    max: 999999999.9
                                    rightText: "MB/s"
                                    hasRightLabel: true
                                }
                            }

                            SettingsItem {
                                label: "Download Timeout"

                                IntTextField {
                                    min: 5000
                                    max: 999999999
                                    rightText: "ms"
                                    hasRightLabel: true
                                }
                            }
                        }
                    }

                    SettingsMajorSection {
                        label: "Platforms"

                        SettingsMinorSection {
                            id: sectionYouTube
                            label: "YouTube"

                            SettingsItem {
                                label: "Cookies"

                                // TODO: Figure out this input
                                // Probably will have a file location rather than the upload like v1
                            }

                            SettingsItem {
                                label: "PO Token"

                                CustomTextField {}
                                // TODO: Add clear, paste, extra buttons like v1
                            }
                        }
                    }

                    SettingsMajorSection {
                        label: "Interface"

                        SettingsMinorSection {
                            id: sectionInterfaceGeneral
                            label: "General"

                            SettingsItem {
                                label: "Show Status Notifications"

                                CustomSwitch {}
                            }

                            SettingsItem {
                                label: "Auto Open Download Folder"

                                CustomSwitch {}
                            }
                        }

                        SettingsMinorSection {
                            id: sectionUpdates
                            label: "Updates"

                            SettingsItem {
                                label: "Check For Updates"

                                CustomSwitch {}
                            }

                            SettingsItem {
                                label: "Check For Notices"

                                CustomSwitch {}
                            }
                        }
                    }
                }
            }
        }
    }
}