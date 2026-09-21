import QtQuick
import QtQuick.Layouts

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