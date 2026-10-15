import QtQuick
import QtQuick.Controls
import QtQuick.Effects

Button {
    id: root

    property color textColour: "white"
    property color backgroundColour: "white"
    property color hoveredColour: Qt.darker(backgroundColour, 1.2)
    property color pressedColour: Qt.darker(backgroundColour, 1.5)
    property real radius: 3

    implicitHeight: parent.height

    font.family: "Roboto"
    font.pixelSize: 14

    // Text
    contentItem: Text {
        text: root.text
        font: root.font
        opacity: enabled ? 1.0 : 0.3
        color: root.textColour
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    // Background
    background: Rectangle {
        color: root.down ? root.pressedColour : (root.hovered ? root.hoveredColour : root.backgroundColour)
        radius: root.radius
    }

    // Outer Shadow
    layer.enabled: true
    layer.effect: MultiEffect {
        shadowEnabled: true
        shadowColor: "black"
        shadowOpacity: 0.25
        shadowBlur: 0.4
        shadowHorizontalOffset: 2
        shadowVerticalOffset: 2
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onPressed: (mouse) => { mouse.accepted = false }
    }
}