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

        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 100
            Layout.rightMargin: 100

            TextField {
                Layout.fillWidth: true
                placeholderText: qsTr("Folder Path")
                text: _manager.folderInputText
            }

            Button {
                Layout.preferredWidth: 30
                icon.source: "qrc:/icons/Icons/Folder_Icon_B.png"
                onClicked: _manager.folderButtonClicked()
            }
        }

        Button {
            Layout.fillWidth: true
            Layout.leftMargin: 100
            Layout.rightMargin: 100

            text: qsTr("ytdlp: Download Test Video")
            onClicked: _manager.ytdlpButtonClicked()
        }

        Button {
            Layout.fillWidth: true
            Layout.leftMargin: 100
            Layout.rightMargin: 100

            text: qsTr("ffmpeg: Convert To MP3")
            onClicked: _manager.ffmpegButtonClicked()
        }

        Button {
            Layout.fillWidth: true
            Layout.leftMargin: 100
            Layout.rightMargin: 100

            text: qsTr("taglib: Add Metadata")
            onClicked: _manager.taglibButtonClicked()
        }

        // Filler
        Rectangle {
            Layout.fillHeight: true
        }
    }
}