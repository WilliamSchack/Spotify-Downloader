import QtQuick

Column {
    property alias label: headerText.text
    default property alias content: settings.children

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
        id: settings
        width: parent.width
        spacing: 10
    }
}