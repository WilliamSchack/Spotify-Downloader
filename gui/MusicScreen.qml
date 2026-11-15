import QtQuick
import QtQuick.Controls.Basic
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

                // Download / Cancel
                CustomButton {
                    Layout.fillHeight: true

                    enabled: _manager.music.downloadButtonEnabled

                    text: _manager.music.downloadButtonText
                    font.pixelSize: 14
                    spacing: 5
                    radius: 8
                    backgroundColour: "#A16583"

                    iconSource: _manager.music.downloadButtonIcon
                    iconMargins: 15

                    onClicked: _manager.music.DownloadButtonClicked()
                }

                // Pause
                CustomButton {
                    Layout.fillHeight: true

                    text: _manager.music.playPauseButtonText
                    font.pixelSize: 14
                    spacing: 5
                    radius: 8
                    backgroundColour: "#A16583"

                    iconSource: _manager.music.playPauseButtonIcon
                    iconMargins: 15

                    onClicked: _manager.music.PlayPauseButtonClicked()
                }

                Rectangle { Layout.fillWidth: true }

                Text {
                    text: "Downloads Completed: " + _manager.music.downloadsCompleted + "/" + _manager.music.tracks.length + " (" + _manager.music.downloadErrors + " Error" + (_manager.music.downloadErrors === 1 ? "" : "s") + ")"
                    font.pixelSize: 14
                    color: "white"
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
            QtObject {
                id: columnWidthsObject
                property real number: 0.05
                property real title: 0.25
                property real album: 0.25
                property real sources: 0.15
                property real downloadStatus: 0.3
            }

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                // Progress Bar
                ProgressBar {
                    id: progressBar

                    Layout.fillWidth: true
                    Layout.preferredHeight: 5

                    value: _manager.music.tracks.length > 0 ? (_manager.music.downloadsCompleted / _manager.music.tracks.length) : 0

                    background: Rectangle {
                        color: Qt.alpha('#6a6a6a', 0.2)
                    }

                    contentItem: Item {
                        visible: _manager.music.tracks.length > 0

                        Rectangle {
                            width: progressBar.width * progressBar.visualPosition
                            height: progressBar.height
                            color: Qt.alpha('#4DFF36', 0.4)
                        }
                    }

                    Behavior on value {
                        NumberAnimation {
                            duration: 500
                            easing.type: Easing.InOutCubic
                        }
                    }
                }

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
                            width: parent.width * columnWidthsObject.number
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
                            width: parent.width * columnWidthsObject.title
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
                            width: parent.width * columnWidthsObject.album
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
                            width: parent.width * columnWidthsObject.sources
                            height: parent.height
                            
                            Text {
                                anchors.verticalCenter: parent.verticalCenter
                                text: "Sources"
                                color: "white"
                                font.pixelSize: 14
                            }
                        }

                        // Download Status
                        Item {
                            width: parent.width * columnWidthsObject.downloadStatus
                            height: parent.height
                            
                            Text {
                                anchors.verticalCenter: parent.verticalCenter
                                text: "Download Status"
                                color: "white"
                                font.pixelSize: 14
                            }
                        }
                    }
                }

                // Tracks
                ListView {
                    id: trackList
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    spacing: 0

                    model: _manager.music.tracks

                    property bool scrollBarActive: contentHeight > height

                    ScrollBar.vertical: ScrollBar {
                        policy: trackList.scrollBarActive ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
                    }

                    delegate: Track {
                        required property QtObject modelData
                        trackData: modelData
                        columnWidths: columnWidthsObject
                        width: ListView.view.width
                    }
                }
            }
        }
    }

    // Popup
    Rectangle {
        anchors.fill: parent
        color: Qt.alpha("#000000", 0.4)
        radius: parent.radius
        opacity: addTracksPopup.visible ? 1.0 : 0.0
        visible: opacity > 0

        // Consume mouse input
        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onPressed: (mouse) => { mouse.accepted = true }
        }

        Behavior on opacity {
            NumberAnimation {
                duration: addTracksPopup.fadeDurationMs
            }
        }
    }

    AddTracksPopup {
        id: addTracksPopup
    }
}