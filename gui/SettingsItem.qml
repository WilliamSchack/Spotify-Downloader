import QtQuick
import QtQuick.Layouts

RowLayout {
    id: root

    width: parent.width
    height: 25
    spacing: 10

    property alias label: text.text
    default property alias content: inputs.children
    
    Text {
        id: text
        Layout.alignment: Qt.AlignVCenter
        color: "white"
        font.pixelSize: 14
    }

    // Filler
    Rectangle { Layout.fillWidth: true }

    RowLayout {
        id: inputs
        Layout.alignment: Qt.AlignVCenter
        spacing: 10
    }
}