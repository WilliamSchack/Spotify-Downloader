import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import QtQuick.Effects

Popup {
    id: root

    property int fadeDurationMs: 100

    anchors.centerIn: parent
    visible: _manager.music.addTracksPopup.visible && (parent ? parent.visible : false)

    modal: false
    dim: false
    focus: true
    closePolicy: Popup.NoAutoClose

    padding: 20
    width: 560 + padding * 2

    Component {
        id: messageDialogComponent
        CustomMessageDialog {}
    }

    background: StyledBackground {
        radius: 20

        // Outer Shadow
        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: "black"
            shadowOpacity: 0.25
            shadowBlur: 0.4
            shadowHorizontalOffset: 4
            shadowVerticalOffset: 4
        }
    }

    contentItem: ColumnLayout {
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

                    iconSource: "qrc:/icons/ui/Clipboard.svg"
                    iconColour: "black"

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

                    iconSource: "qrc:/icons/ui/Folder.svg"
                    iconColour: "black"

                    onClicked: _manager.music.addTracksPopup.FolderButtonClicked()
                }
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
                backgroundColour: '#9A607D'

                onClicked: _manager.music.addTracksPopup.visible = false
            }
        }
    }

    enter: Transition {
        NumberAnimation {
            property: "opacity"
            from: 0.0
            to: 1.0
            duration: root.fadeDurationMs
        }
    }

    exit: Transition {
        NumberAnimation {
            property: "opacity"
            from: 1.0
            to: 0.0
            duration: root.fadeDurationMs
        }
    }

    Connections {
        target: _manager.music.addTracksPopup
        function onStartError(error) {
            console.log("ON ERROR")

            const dialog = messageDialogComponent.createObject(root, {
                title: "Could Not Start Download",
                text: error,
                standardButtons: Dialog.Ok
            })
            dialog.open()
        }
    }
}