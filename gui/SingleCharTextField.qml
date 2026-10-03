import QtQuick
import QtQuick.Controls

TextField {
    implicitHeight: parent.height
    implicitWidth: implicitHeight

    padding: 0
    leftPadding: 0 // Needs to be set for text to be centered for some reason

    horizontalAlignment: TextInput.AlignHCenter
    verticalAlignment: TextInput.AlignVCenter

    font.pixelSize: 14
    color: "black"

    maximumLength: 1

    background: Rectangle {
        width: parent.width
        radius: 3
    }
}