import QtQuick
import QtQuick.Layouts

ToggleButton {
    id: root

    required property var info

    implicitWidth: parent.width
    height: 50

    unselectedBackgroundColour: "transparent"
    selectedBackgroundColour: Qt.alpha("white", 0.3)

    radius: 6

    onClicked: _manager.info.notices.SelectNotice(info.index)

    ColumnLayout {
        anchors.fill: parent

        anchors.topMargin: 5
        anchors.bottomMargin: 8

        Text {
            Layout.fillWidth: true
            Layout.fillHeight: true

            text: root.info.title
            color: "white"
            font.pixelSize: 16
            font.bold: true

            elide: Text.ElideRight
        }

        Text {
            Layout.fillWidth: true
            Layout.fillHeight: true

            text: root.info.releaseDate
            color: "white"
            font.pixelSize: 12
            font.italic: true
        }
    }

    Component.onCompleted: {
        // Check this initially if its the first one
        checked = root.info.index === 0
    }
}