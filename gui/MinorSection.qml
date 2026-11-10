import QtQuick

Column {
    property alias label: headerText.text
    default property alias content: items.children

    width: parent.width
    spacing: 10

    // Subheader
    Text {
        id: headerText
        color: "white"
        font.bold: true
        font.pixelSize: 14
    }

    // Items
    Column {
        id: items
        width: parent.width
        spacing: 10
    }
}