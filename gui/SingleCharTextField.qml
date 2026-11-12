import QtQuick
import QtQuick.Controls.Basic

CustomTextField {
    implicitHeight: parent.height
    implicitWidth: implicitHeight

    leftPadding: 0
    rightPadding: 0

    horizontalAlignment: TextInput.AlignHCenter
    verticalAlignment: TextInput.AlignVCenter

    font.pixelSize: 14
    color: "black"

    maximumLength: 1
}