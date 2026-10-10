import QtQuick
import QtQuick.Controls

// TODO: Visibly disable when enabled is false

TextField {
    id: root
    implicitHeight: parent.height
    implicitWidth: 300

    topPadding: 0
    bottomPadding: 0
    leftPadding: 8
    rightPadding: hasRightLabel ? rightLabel.width + rightLabel.anchors.rightMargin * 2 : 8

    font.pixelSize: 14
    color: enabled ? "black" : "grey"

    property bool hasRightLabel: false
    property bool valid: true
    property alias rightText: rightLabel.text

    background: Rectangle {
        width: root.width
        radius: 3

        color: root.valid ? "white" : '#ffdedd'

        Behavior on color {
            ColorAnimation {
                duration: 200
                easing.type: Easing.OutCubic
            }
        }
    }

    Text {
        id: rightLabel
        visible: root.hasRightLabel
        color: root.color
        font: root.font
        anchors.right: root.right
        anchors.rightMargin: 8
        anchors.verticalCenter: root.verticalCenter
    }
}