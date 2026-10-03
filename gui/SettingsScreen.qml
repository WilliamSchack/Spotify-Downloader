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

        // Will be changed to a list later
        Column {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 20

            // Header
            Column {
                width: parent.width
                height: 34
                spacing: 10

                Text {
                    text: "Output"
                    color: "white"
                    font.bold: true
                    font.pixelSize: 20
                }

                Rectangle {
                    width: parent.width
                    height: 1
                    color: "#E2A5C4"
                }
            }

            // Each subsection has its own column for lower spacing
            Column {
                width: parent.width
                spacing: 10

                // Subheader
                Text {
                    text: "Audio"
                    color: "white"
                    font.bold: true
                    font.pixelSize: 14
                }

                // Each setting should be able to have multiple of the below inputs stacked in a row

                // Dropdown
                RowLayout {
                    width: parent.width
                    height: 30

                    Text {
                        Layout.alignment: Qt.AlignVCenter
                        text: "Codec"
                        color: "white"
                        font.pixelSize: 14
                    }

                    // Filler
                    Rectangle { Layout.fillWidth: true }

                    // Dropdown
                    CustomComboBox {
                        Layout.fillHeight: true

                        model: ["MP3", "M4A", "WAV"/*, ...*/]
                    }
                }

                // Input
                RowLayout {
                    width: parent.width
                    height: 30

                    Text {
                        Layout.alignment: Qt.AlignVCenter
                        text: "Normalise Volume"
                        color: "white"
                        font.pixelSize: 14
                    }

                    // Filler
                    Rectangle { Layout.fillWidth: true }

                    // Input
                    NumberTextField {
                        Layout.fillHeight: true
                        
                        min: -50.0
                        max: 50.0
                        decimals: 1
                        rightText: "dB"
                        hasRightLabel: true
                    }
                }

                // Single Input
                RowLayout {
                    width: parent.width
                    height: 30
                    
                    Text {
                        Layout.alignment: Qt.AlignVCenter
                        text: "Single Input"
                        color: "white"
                        font.pixelSize: 14
                    }

                    // Filler
                    Rectangle { Layout.fillWidth: true }

                    // Single Input
                    SingleCharTextField {
                        Layout.fillHeight: true
                        Layout.preferredWidth: parent.height
                    }
                }

                // Toggle Button
                RowLayout {
                    width: parent.width
                    height: 30
                    
                    Text {
                        Layout.alignment: Qt.AlignVCenter
                        text: "Codec"
                        color: "white"
                        font.pixelSize: 14
                    }

                    // Filler
                    Rectangle { Layout.fillWidth: true }

                    // Toggle Button
                    CustomSwitch {
                        Layout.fillHeight: true
                    }
                }
            }
        }
    }
}