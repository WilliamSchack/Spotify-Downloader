import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    required property bool isLoading
    required property bool loadingFailed
    required property string contentLabel

    anchors.fill: parent

    Row {
        anchors.centerIn: parent
        visible: root.isLoading
        spacing: 10

        BusyIndicator {
            running: root.isLoading
            palette.dark: "white"
            height: 50
            width: 50
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: "Loading " + root.contentLabel + "..."
            color: "white"
            font.bold: true
            font.pixelSize: 28
        }
    }

    Text {
        anchors.centerIn: parent
        visible: root.loadingFailed
        text: "Failed to Get " + contentLabel
        color: "white"
        font.bold: true
        font.pixelSize: 28
    }
}