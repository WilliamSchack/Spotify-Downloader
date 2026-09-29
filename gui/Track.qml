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
            
            RowLayout {
                width: parent.width
                spacing: 10

                // Cover Art
                Image {
                    Layout.preferredWidth: 30
                    Layout.preferredHeight: 30
                    Layout.alignment: Qt.AlignVCenter

                    source: root.trackData.coverArtUrl
                    sourceSize.width: width * 2
                    sourceSize.height: height * 2
                    asynchronous: true
                    fillMode: Image.PreserveAspectCrop
                }

                Column {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    // Title
                    Text {
                        width: parent.width
                        rightPadding: 20
                        text: root.trackData.name
                        color: "white"
                        font.pixelSize: 14
                        elide: Text.ElideRight
                    }

                    // Artist
                    Text {
                        width: parent.width
                        rightPadding: 20
                        text: root.trackData.artistNames
                        color: "white"
                        font.pixelSize: 12
                        elide: Text.ElideRight
                    }
                }
            }
        }

        // Album
        Item {
            width: root.columnWidths.album
            height: parent.height
            Text {
                width: parent.width
                anchors.verticalCenter: parent.verticalCenter
                rightPadding: 20

                text: root.trackData.albumName
                color: "white"
                font.pixelSize: 14
                elide: Text.ElideRight
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