// TEMPORARY, only used for repeaters
pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

Rectangle {
    radius: 10
    color: "transparent"

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
                
                color: Qt.alpha("#262626", 0.9)
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
                    text: "Download Tracks (4/8)"
                    font.pixelSize: 14
                    spacing: 5
                    radius: 8
                    color: "#A16583"
                    
                    Layout.fillHeight: true
                }

                Button {
                    text: "Export"
                    font.pixelSize: 14
                    spacing: 5
                    radius: 8
                    color: "#A16583"
                    
                    Layout.fillHeight: true
                }

                Rectangle { Layout.fillWidth: true }

                // Search should be changed to its own thing
                Button {
                    text: "Search"
                    font.pixelSize: 14
                    spacing: 5
                    radius: 8
                    color: Qt.alpha("#707070", 0.4)
                    
                    Layout.preferredWidth: 150
                    Layout.fillHeight: true
                }

                Button {
                    text: "Track Number"
                    font.pixelSize: 14
                    spacing: 5
                    radius: 8
                    iconRight: true
                    color: Qt.alpha("#707070", 0.4)
                    
                    Layout.fillHeight: true
                }

                Button {
                    text: "Filter"
                    font.pixelSize: 14
                    spacing: 5
                    radius: 8
                    iconRight: true
                    color: Qt.alpha("#707070", 0.4)
                    
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

            gradient: Gradient {
                GradientStop { position: 0.0; color: Qt.alpha("#373737", 0.8) }
                GradientStop { position: 1.0; color: Qt.alpha("#3D2B3F", 0.8) }
            }

            // Column Widths (Will be adjustable later)
            // Idealy change these to a percentage of the window width
            QtObject {
                id: columnWidths
                property real number: 30
                property real title: 200
                property real album: 200
                property real sources: 100
            }

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                // Top Section
                Rectangle {
                    Layout.fillWidth: true
                    Layout.minimumHeight: 40

                    color: "transparent"

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
                        Track {}
                    }
                }

                Rectangle { Layout.fillHeight: true }
            }
        }
    }
}