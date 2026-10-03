import QtQuick
import QtQuick.Controls

TextField {
    id: root

    property alias min: validatorId.bottom
    property alias max: validatorId.top
    property alias decimals: validatorId.decimals

    property bool hasRightLabel: false
    property alias rightText: rightLabel.text

    implicitHeight: parent.height
    implicitWidth: 300

    topPadding: 0
    bottomPadding: 0
    leftPadding: 8
    rightPadding: hasRightLabel ? rightLabel.width + rightLabel.anchors.rightMargin * 2 : 8

    font.pixelSize: 14
    color: "black"

    validator: DoubleValidator {
        id: validatorId
        bottom: -50.0
        top: 50.0
        decimals: 1
        notation: DoubleValidator.StandardNotation
    }

    background: Rectangle {
        width: root.width
        radius: 3
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