import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Rectangle {
    id: root
    anchors.fill: parent
    visible: _manager.music.addTracksPopup.visible

    // Background Dim
    color: Qt.alpha("#000000", 0.4)

    // Popup
    Rectangle {
        anchors.centerIn: parent
        width: 600
        height: 231

        radius: 20

        gradient: Gradient {
            GradientStop { position: 0.0; color: Qt.alpha("#252126", 0.8) }
            GradientStop { position: 1.0; color: Qt.alpha("#3E2C40", 0.8) }
        }

        border.color: "#F8A8CE"
        border.width: 1

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 10

            Text {
                Layout.fillWidth: true
                horizontalAlignment: Qt.AlignHCenter

                text: "Add Tracks"
                color: "white"
                font.bold: true
                font.pixelSize: 30
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 5

                // Download Link
                RowLayout {
                    Layout.fillWidth: true
                    Layout.maximumHeight: 40
                    spacing: 5

                    TextField {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        placeholderText: qsTr("Enter a Song/Album/Playlist URL...")
                        font.pixelSize: 18

                        verticalAlignment: TextInput.AlignVCenter
                        leftPadding: 10

                        background: Rectangle {
                            radius: 3
                            color: "white"
                        }

                        text: _manager.music.addTracksPopup.linkInputText
                        onTextChanged: _manager.music.addTracksPopup.linkInputText = text
                    }

                    CustomButton {
                        Layout.preferredWidth: 40
                        Layout.preferredHeight: 40

                        radius: 3
                        hasIcon: false
                        color: "white"
                        //icon.source: "qrc:/icons/Icons/Clipboard_Icon_B.png"
                        onClicked: _manager.music.addTracksPopup.PasteButtonClicked()
                    }
                }

                // Folder Path
                RowLayout {
                    Layout.fillWidth: true
                    Layout.maximumHeight: 40
                    spacing: 5

                    TextField {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        placeholderText: qsTr("Select a Save Location ...")
                        font.pixelSize: 18

                        verticalAlignment: TextInput.AlignVCenter
                        leftPadding: 10

                        background: Rectangle {
                            radius: 3
                            color: "white"
                        }

                        text: _manager.music.addTracksPopup.folderInputText
                        onTextChanged: _manager.music.addTracksPopup.folderInputText = text
                    }

                    CustomButton {
                        Layout.preferredWidth: 40
                        Layout.preferredHeight: 40
                        width: height

                        radius: 3
                        hasIcon: false
                        color: "white"
                        //icon.source: "qrc:/icons/Icons/Folder_Icon_B.png"
                        onClicked: _manager.music.addTracksPopup.FolderButtonClicked()
                    }
                }
            }

            CustomButton {
                Layout.fillWidth: true
                Layout.preferredHeight: 40
                hasIcon: false
                text: "Add Track(s) & Start Download"
                font.bold: true
                font.pixelSize: 16
                color: "#9A607D"

                onClicked: _manager.music.addTracksPopup.DownloadButtonClicked()
            }
        }
    }
}