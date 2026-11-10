import QtQuick

Column
{
    property alias label: headerText.text

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