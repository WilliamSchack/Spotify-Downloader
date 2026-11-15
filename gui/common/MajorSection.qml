import QtQuick

Column {
    property alias label: header.label
    default property alias content: items.children

    width: parent.width
    spacing: 20

    // Header
    Header {
        id: header
    }

    // Items
    Column {
        id: items
        width: parent.width
        spacing: 20
    }
}