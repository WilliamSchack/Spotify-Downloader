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

            Row {
                required property var modelData

                width: root.width
                spacing: 10

                Text {
                    text: modelData.name
                    color: "white"
                    font.pixelSize: 16
                    font.bold: true
                }

                Text {
                    text: modelData.timeString
                    color: "white"
                    font.pixelSize: 16
                }
            }
        }
    }
}