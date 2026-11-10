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

        Text {
            Layout.fillWidth: true

            text: root.info.title
            color: "white"
            font.pixelSize: 12
            font.italic: true

            elide: Text.ElideRight
        }

        Item { Layout.fillWidth: true }

        Text {
            text: root.info.releaseDate
            color: "white"
            font.pixelSize: 12
            font.italic: true
        }
    }
}