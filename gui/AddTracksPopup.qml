import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic

Rectangle {
    id: root

    property real margins: 20

    anchors.fill: parent
    visible: _manager.music.addTracksPopup.visible

    // Background Dim
    color: Qt.alpha("#000000", 0.4)

    // Steal mouse inputs
    MouseArea {
        anchors.fill: parent
        onPressed: (mouse) => { mouse.accepted = true }
    }

    // Popup
    StyledBackground {
        anchors.centerIn: parent
        width: 560 + root.margins * 2
        height: content.implicitHeight + root.margins * 2

        radius: 20
        
        ColumnLayout {
            id: content

            anchors.fill: parent
            anchors.margins: root.margins
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

                    CustomTextField {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        placeholderText: qsTr("Enter a Song/Album/Playlist URL...")
                        font.pixelSize: 18

                        verticalAlignment: TextInput.AlignVCenter
                        leftPadding: 10

                        text: _manager.music.addTracksPopup.linkInputText
                        onTextChanged: _manager.music.addTracksPopup.linkInputText = text
                    }

                    CustomButton {
                        Layout.preferredWidth: 40
                        Layout.preferredHeight: 40

                        display: AbstractButton.IconOnly
                        //icon.source: "qrc:/icons/Icons/Clipboard_Icon_B.png"
                        onClicked: _manager.music.addTracksPopup.PasteButtonClicked()
                    }
                }

                // Folder Path
                RowLayout {
                    Layout.fillWidth: true
                    Layout.maximumHeight: 40
                    spacing: 5

                    CustomTextField {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        placeholderText: qsTr("Select a Save Location...")
                        font.pixelSize: 18

                        verticalAlignment: TextInput.AlignVCenter
                        leftPadding: 10

                        text: _manager.music.addTracksPopup.folderInputText
                        onTextChanged: _manager.music.addTracksPopup.folderInputText = text
                    }

                    CustomButton {
                        Layout.preferredWidth: 40
                        Layout.preferredHeight: 40

                        display: AbstractButton.IconOnly
                        //icon.source: "qrc:/icons/Icons/Folder_Icon_B.png"
                        onClicked: _manager.music.addTracksPopup.FolderButtonClicked()
                    }
                }
            }

            // Error popup
            Label {
                Layout.fillWidth: true
                
                visible: _manager.music.addTracksPopup.errorMessage != ""

                padding: 10

                text: _manager.music.addTracksPopup.errorMessage
                color: "white"
                font.pixelSize: 16

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                background: Rectangle {
                    radius: 3

                    gradient: Gradient {
                        GradientStop { position: 0.0; color: Qt.alpha('#9f5151', 0.8) }
                        GradientStop { position: 1.0; color: Qt.alpha('#773131', 0.8) }
                    }

                    border.color: '#c56c6c'
                    border.width: 3
                }
            }

            ColumnLayout {
                Layout.fillWidth: true

                spacing: 5

                CustomButton {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    enabled: !_manager.music.addTracksPopup.checkingInputs
                    text: _manager.music.addTracksPopup.checkingInputs ? "Getting Link Details..." : "Add Track(s) & Start Download"
                    font.bold: true
                    font.pixelSize: 16
                    backgroundColour: "#9A607D"

                    onClicked: _manager.music.addTracksPopup.DownloadButtonClicked()
                }

                CustomButton {
                    visible: _manager.music.tracks.length != 0

                    Layout.fillWidth: true
                    Layout.preferredHeight: 30
                    enabled: !_manager.music.addTracksPopup.checkingInputs
                    text: "Close"
                    font.bold: true
                    font.pixelSize: 16
                    backgroundColour: '#808080'

                    onClicked: _manager.music.addTracksPopup.visible = false
                }
            }
        }
    }
}