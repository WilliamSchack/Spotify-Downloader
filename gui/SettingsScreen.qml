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
                id: settingsFlickable
                anchors.fill: parent
                clip: true

                contentWidth: width
                contentHeight: settingsColumn.height

                property bool scrollBarActive: contentHeight > height

                ScrollBar.vertical: ScrollBar {
                    policy: settingsFlickable.scrollBarActive ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
                }

                Column {
                    id: settingsColumn
                    width: parent.width - (settingsFlickable.scrollBarActive ? 20 : 0)
                    spacing: 20

                    SettingsMajorSection {
                        label: "Output"

                        SettingsMinorSection {
                            label: "Audio"

                            SettingsItem {
                                label: "Codec"

                                CustomComboBox {
                                    model: _manager.settings.GetCodecOptions()
                                    currentIndex: _manager.settings.codec
                                    onActivated: function(index) { _manager.settings.codec = index }
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
                                    onTextEdited: function() { _manager.settings.normaliseDb = parseFloat(text) }
                                }
                            }

                            SettingsItem {
                                label: "Audio Bitrate"

                                CustomComboBox {
                                    width: 150

                                    model: ["Best Quality", "Manual"]

                                    currentIndex: _manager.settings.manualBitrate ? 1 : 0
                                    onActivated: function(index) { _manager.settings.manualBitrate = index == 1 }
                                }

                                IntTextField {
                                    // Should change based on codec, premium status, and auto update when manualBitrate (Same as v1)
                                    enabled: _manager.settings.manualBitrate
                                    min: 0
                                    max: 128
                                    rightText: "kb/s"
                                    hasRightLabel: true

                                    text: _manager.settings.bitrate
                                    onTextEdited: function(index) { _manager.settings.bitrate = parseInt(text) }
                                }
                            }
                        }

                        SettingsMinorSection {
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
                                    onTextEdited: function() { _manager.settings.artistsSeperator = text }
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
                            label: "File Management"

                            SettingsItem {
                                label: "Overwrite"

                                CustomSwitch {}
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
                            label: "General"

                            SettingsItem {
                                label: "Max Simultaneous Downloads"

                                IntTextField {
                                    min: 0
                                    max: 128

                                    text: _manager.settings.downloadThreads
                                    onTextEdited: function() { _manager.settings.downloadThreads = text }
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