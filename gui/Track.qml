import QtQuick
import QtQuick.Layouts

Rectangle {
    id: root

    required property QtObject trackData
    required property QtObject columnWidths
    
    height: 40
    color: "transparent"

    Row {
        anchors.fill: parent
        spacing: 0

        // Number
        Item {
            width: root.columnWidths.number
            height: parent.height
            Text {
                anchors.centerIn: parent
                text: root.trackData.trackNumber
                color: "white"
                font.pixelSize: 12
            }
        }

        // Title / Artist
        Item {
            width: root.columnWidths.title
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
                        text: root.trackData.name
                        color: "white"
                        font.pixelSize: 14
                    }

                    // Artist
                    Text {
                        text: root.trackData.artistNames
                        color: "white"
                        font.pixelSize: 12
                    }
                }
            }
        }

        // Album
        Item {
            width: root.columnWidths.album
            height: parent.height
            Text {
                anchors.verticalCenter: parent.verticalCenter

                text: root.trackData.albumName
                color: "white"
                font.pixelSize: 14
            }
        }

        // Sources
        Item {
            width: root.columnWidths.sources
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