import QtQuick

MajorSection {
    id: root

    required property var info

    width: parent.width

    label: info.title

    Column {
        width: parent.width
        spacing: 6

        Repeater {
            model: info.supporters

            Text {
                required property var modelData

                width: root.width

                text: modelData.name
                color: "white"
                font.pixelSize: 14
            }
        }
    }
}