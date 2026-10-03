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
        
        Column {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 20

            SettingsMajorSection {
                label: "Output"

                SettingsSubSection {
                    label: "Audio"

                    SettingsItem {
                        label: "Codec"

                        CustomComboBox {
                            Layout.fillHeight: true

                            model: ["MP3", "M4A", "WAV"/*, ...*/]
                        }
                    }

                    SettingsItem {
                        label: "Normalise Volume"

                        CustomSwitch {
                            width: 100
                        }

                        NumberTextField {
                            Layout.fillHeight: true
                            
                            min: -50.0
                            max: 50.0
                            decimals: 1
                            rightText: "dB"
                            hasRightLabel: true
                        }
                    }

                    SettingsItem {
                        label: "Audio Bitrate"

                        CustomComboBox {
                            Layout.fillHeight: true
                            Layout.preferredWidth: 150

                            model: ["Best Quality", "Manual"/*, ...*/]
                        }

                        NumberTextField {
                            Layout.fillHeight: true
                            
                            min: 0
                            max: 128 // Should change based on codec and premium status (Same as v1)
                            decimals: 0
                            rightText: "kb/s"
                            hasRightLabel: true
                        }
                    }
                }
            }
        }
    }
}