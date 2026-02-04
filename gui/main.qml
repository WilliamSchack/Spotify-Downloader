import QtQuick 2.9
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

Window
{
    visible: true
    width: 640
    height: 480
    title: qsTr("Minimal Qml")

    // Background
    Rectangle {
        anchors.fill: parent
        color: "lightblue"
    }

    ColumnLayout {
        anchors.fill: parent

        // Filler
        Rectangle {
            Layout.fillHeight: true
        }

        // Download Link
        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 100
            Layout.rightMargin: 100

            TextField {
                Layout.fillWidth: true
                placeholderText: qsTr("Download Link")
                text: _manager.linkInputText
                onTextChanged: _manager.linkInputText = text
            }

            Button {
                Layout.preferredWidth: 30
                icon.source: "qrc:/icons/Icons/Clipboard_Icon_B.png"
                onClicked: _manager.PasteButtonClicked()
            }
        }

        // Folder Path
        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 100
            Layout.rightMargin: 100

            TextField {
                Layout.fillWidth: true
                placeholderText: qsTr("Folder Path")
                text: _manager.folderInputText
                onTextChanged: _manager.folderInputText = text
            }

            Button {
                Layout.preferredWidth: 30
                icon.source: "qrc:/icons/Icons/Folder_Icon_B.png"
                onClicked: _manager.FolderButtonClicked()
            }
        }

        Button {
            Layout.fillWidth: true
            Layout.leftMargin: 100
            Layout.rightMargin: 100

            text: qsTr("Download")
            onClicked: _manager.DownloadButtonClicked()
        }

        // Filler
        Rectangle {
            Layout.fillHeight: true
        }
    }
}