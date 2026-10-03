import QtQuick

Column {
    property alias label: headerText.text
    default property alias content: settings.children

    width: parent.width
    spacing: 20

    // Header
    Column {
        width: parent.width
        height: 34
        spacing: 10

        Text {
            id: headerText
            color: "white"
            font.bold: true
            font.pixelSize: 20
        }

        Rectangle {
            width: parent.width
            height: 1
            color: "#E2A5C4"
        }
    }

    // Items
    Column {
        id: settings
        width: parent.width
        spacing: 20
    }
}