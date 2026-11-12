import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
 
Rectangle {
    id: root

    required property QtObject trackData
    required property QtObject columnWidths
    property string progressBarColour: Qt.alpha('#4DFF36', 0.4)

    height: 40
    color: "transparent"

    // Progress Gradient
    Rectangle {
        width: parent.width * root.trackData.progress
        height: parent.height

        visible: root.trackData.progress > 0 && root.trackData.progress < 1
        color: root.progressBarColour
    }

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
                height: parent.height

                Layout.alignment: Qt.AlignHCenter

                spacing: 10

                // TODO: Fix error "Corrupt JPEG data: premature end of data segment"

                // Cover Art
                Item {
                    Layout.preferredWidth: 30
                    Layout.preferredHeight: 30
                    Layout.alignment: Qt.AlignVCenter

                    // Loaded
                    RoundedImage {
                        anchors.fill: parent
                        visible: root.trackData.coverArtUrl !== ""

                        radius: 6
                        source: root.trackData.coverArtUrl
                        sourceSize.width: width * 2
                        sourceSize.height: height * 2
                        asynchronous: true
                        fillMode: Image.PreserveAspectCrop
                    }

                    // Loading
                    Item {
                        anchors.fill: parent
                        visible: root.trackData.coverArtUrl === ""

                        Rectangle {
                            anchors.fill: parent

                            radius: 6
                            color: "#656565"
                        }

                        BusyIndicator {
                            anchors.fill: parent

                            running: parent.visible
                            palette.dark: "white"
                        }
                    }
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
                spacing: 10

                Repeater {
                    model: root.trackData.sources

                    Image {
                        required property var modelData

                        width: 16
                        height: width
                    
                        source: modelData.iconSource
                        sourceSize.width: width
                        sourceSize.height: height

                        MouseArea {
                            anchors.fill: parent

                            onPressed: {
                                Qt.openUrlExternally(modelData.link)
                                sourceTooltip.close()
                            }
                        }

                        HoverTooltip {
                            id: sourceTooltip
                            text: modelData.link
                        }
                    }
                }
            }
        }

        // Download Status
        Item {
            width: root.columnWidths.downloadStatus
            height: parent.height
            Text {
                width: parent.width
                anchors.verticalCenter: parent.verticalCenter
                rightPadding: 20

                text: root.trackData.status
                color: "white"
                font.pixelSize: 14
                elide: Text.ElideRight
            }
        }
    }

    // Happens first time the track comes into view
    Component.onCompleted: {
        if (trackData.coverArtUrl != "")
            return
        
        trackData.LoadCoverArt()
    }
}