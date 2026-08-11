// TEMPORARY, only used for repeaters
pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

Window
{
    visible: true
    width: 1400
    height: 800
    title: qsTr("Minimal Qml")
    color: "purple"

    // Outer Margin
    Rectangle {
        anchors.fill: parent
        anchors.margins: 5
        color: "transparent"

        ColumnLayout {
            anchors.fill: parent
            spacing: 5

            // Header
            RowLayout {
                Layout.maximumHeight: 30
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft
                spacing: 15

                // Buttons (testing)
                Button {
                    text: "Music"
                    Layout.fillHeight: true
                }

                Button {
                    text: "Info"
                    Layout.fillHeight: true
                }

                Button {
                    text: "Settings"
                    Layout.fillHeight: true
                }

                // Space
                Rectangle { Layout.fillWidth: true }

                // Buttons (testing)
                Repeater {
                    model: 3
                    Rectangle {
                        Layout.preferredWidth: parent.height
                        Layout.fillHeight: true

                        radius: 20
                        color: "green"
                    }
                }
            }

            RowLayout {
                Layout.fillHeight: true
                Layout.fillWidth: true
                spacing: 10

                // Left Area
                Rectangle {
                    Layout.fillHeight: true
                    Layout.preferredWidth: 260

                    radius: 10
                    color: "blue"

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 10

                        // Group Header
                        ColumnLayout {
                            spacing: 10

                            RowLayout {
                                Layout.fillWidth: true

                                Text {
                                    text: "Groups"
                                    color: "white"
                                    font.family: "Roboto"
                                    font.bold: true
                                    font.pixelSize: 20
                                }

                                Rectangle { Layout.fillWidth: true }

                                Rectangle {
                                    Layout.preferredHeight: 25
                                    Layout.preferredWidth: 25

                                    radius: 5
                                    color: "green"

                                    Text {
                                        anchors.centerIn: parent
                                        font.family: "Roboto"
                                        font.pixelSize: 20
                                        color: "white"
                                        text: "+"
                                    }
                                }
                            }

                            // Bottom border
                            Rectangle {
                                Layout.preferredHeight: 1
                                Layout.fillWidth: true
                                color: "green"
                            }
                        }

                        // Group Items
                        Repeater {
                            model: 2
                            Rectangle {
                                Layout.minimumHeight: 60
                                Layout.fillWidth: true

                                radius: 6
                                color: "grey"

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 4
                                    anchors.rightMargin: 8
                                    spacing: 10

                                    Rectangle {
                                        Layout.fillHeight: true
                                        Layout.minimumWidth: parent.height

                                        radius: 4
                                        color: "aqua"
                                    }

                                    ColumnLayout {
                                        Layout.fillHeight: true
                                        Layout.fillWidth: true
                                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                                        spacing: 4

                                        Text {
                                            Layout.fillWidth: true

                                            text: "Sample Group"
                                            color: "white"
                                            font.family: "Roboto"
                                            font.pixelSize: 16
                                        }

                                        Text {
                                            Layout.fillWidth: true

                                            text: "8 Tracks"
                                            color: "white"
                                            font.family: "Roboto"
                                            font.pixelSize: 12
                                        }
                                    }

                                    Text {
                                        Layout.fillHeight: true
                                        Layout.alignment: Qt.AlignRight

                                        text: ":"
                                        color: "white"
                                        font.family: "Roboto"
                                        font.pixelSize: 16
                                    }
                                }
                            }
                        }

                        // Space
                        Rectangle {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            color: "transparent"
                        }
                    }
                }

                // Right Area
                Rectangle {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    radius: 10
                    color: "red"

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 0
                        spacing: 0

                        // Header
                        Item {
                            Layout.minimumHeight: 40
                            Layout.fillWidth: true

                            // Background
                            Rectangle {
                                anchors.fill: parent

                                // Qt 6.7+
                                topLeftRadius: 10
                                topRightRadius: 10
                                
                                color: "black"
                            }

                            // Items
                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 10
                                anchors.rightMargin: 10
                                anchors.topMargin: 6
                                anchors.bottomMargin: 6

                                spacing: 8

                                Button {
                                    text: "Sync"
                                    font.pixelSize: 14
                                    spacing: 5
                                    radius: 8

                                    Layout.fillHeight: true
                                }

                                Button {
                                    text: "Add Tracks"
                                    font.pixelSize: 14
                                    spacing: 5
                                    radius: 8
                                    
                                    Layout.fillHeight: true
                                }

                                Button {
                                    text: "Export"
                                    font.pixelSize: 14
                                    spacing: 5
                                    radius: 8
                                    
                                    Layout.fillHeight: true
                                }

                                Button {
                                    text: "Download Tracks (4/8)"
                                    font.pixelSize: 14
                                    spacing: 5
                                    radius: 8
                                    
                                    Layout.fillHeight: true
                                }

                                Rectangle { Layout.fillWidth: true }

                                // Search should be changed to its own thing
                                Button {
                                    text: "Search"
                                    color: "grey"
                                    font.pixelSize: 14
                                    spacing: 5
                                    radius: 8
                                    
                                    Layout.preferredWidth: 150
                                    Layout.fillHeight: true
                                }

                                Button {
                                    text: "Track Number"
                                    color: "grey"
                                    font.pixelSize: 14
                                    spacing: 5
                                    radius: 8
                                    iconRight: true
                                    
                                    Layout.fillHeight: true
                                }

                                Button {
                                    text: "Filter"
                                    color: "grey"
                                    font.pixelSize: 14
                                    spacing: 5
                                    radius: 8
                                    iconRight: true
                                    
                                    Layout.fillHeight: true
                                }
                            }
                        }

                        // Main Area
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            bottomLeftRadius: 10
                            bottomRightRadius: 10

                            // Column Widths (Will be adjustable later)
                            // Idealy change these to a percentage of the window width
                            QtObject {
                                id: columnWidths
                                property real number: 30
                                property real title: 200
                                property real album: 200
                                property real sources: 100
                            }

                            // Top Section
                            ColumnLayout {
                                anchors.fill: parent
                                spacing: 0

                                Rectangle {
                                    Layout.fillWidth: true
                                    Layout.minimumHeight: 40

                                    color: "grey"

                                    Row {
                                        anchors.fill: parent
                                        spacing: 0

                                        // Number
                                        Item {
                                            width: columnWidths.number
                                            height: parent.height
                                            Text {
                                                anchors.centerIn: parent
                                                text: "#"
                                                color: "white"
                                                font.pixelSize: 14
                                            }
                                        }

                                        // Title / Artist
                                        Item {
                                            width: columnWidths.title
                                            height: parent.height
                                            
                                            Text {
                                                anchors.verticalCenter: parent.verticalCenter
                                                text: "Title"
                                                color: "white"
                                                font.pixelSize: 14
                                            }
                                        }

                                        // Album
                                        Item {
                                            width: columnWidths.album
                                            height: parent.height
                                            
                                            Text {
                                                anchors.verticalCenter: parent.verticalCenter
                                                text: "Album"
                                                color: "white"
                                                font.pixelSize: 14
                                            }
                                        }

                                        // Sources
                                        Item {
                                            width: columnWidths.sources
                                            height: parent.height
                                            
                                            Text {
                                                anchors.verticalCenter: parent.verticalCenter
                                                text: "Sources"
                                                color: "white"
                                                font.pixelSize: 14
                                            }
                                        }
                                    }
                                }

                                // Tracks
                                ColumnLayout {
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    spacing: 0

                                    Repeater {
                                        model: 3
                                        Rectangle {
                                            Layout.fillWidth: true
                                            Layout.minimumHeight: 40

                                            color: "green"

                                            Row {
                                                anchors.fill: parent
                                                spacing: 0

                                                // Number
                                                Item {
                                                    width: columnWidths.number
                                                    height: parent.height
                                                    Text {
                                                        anchors.centerIn: parent
                                                        text: "1"
                                                        color: "white"
                                                        font.pixelSize: 12
                                                    }
                                                }

                                                // Title / Artist
                                                Item {
                                                    width: columnWidths.title
                                                    height: parent.height
                                                    
                                                    Row {
                                                        spacing: 10

                                                        // Cover Art
                                                        Rectangle {
                                                            anchors.verticalCenter: parent.verticalCenter
                                                            width: 30
                                                            height: width
                                                            radius: 4
                                                        }

                                                        Column {
                                                            // Title
                                                            Text {
                                                                text: "no complaints"
                                                                color: "white"
                                                                font.pixelSize: 14
                                                            }

                                                            // Artist
                                                            Text {
                                                                text: "BONESAW"
                                                                color: "white"
                                                                font.pixelSize: 12
                                                            }
                                                        }
                                                    }
                                                }

                                                // Album
                                                Item {
                                                    width: columnWidths.album
                                                    height: parent.height
                                                    Text {
                                                        anchors.verticalCenter: parent.verticalCenter

                                                        text: "no complaints"
                                                        color: "white"
                                                        font.pixelSize: 14
                                                    }
                                                }

                                                // Sources
                                                Item {
                                                    width: columnWidths.sources
                                                    height: parent.height

                                                    Row {
                                                        anchors.verticalCenter: parent.verticalCenter
                                                        spacing: 6
                                                        Repeater {
                                                            model: 2
                                                            Rectangle {
                                                                width: 15
                                                                height: width
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }

                                Rectangle { Layout.fillHeight: true }
                            }
                        }
                    }
                }
            }
        }
    }
}