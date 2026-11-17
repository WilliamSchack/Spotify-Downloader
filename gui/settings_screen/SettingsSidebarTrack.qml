import QtQuick

Rectangle {
    id: root
    implicitWidth: 3
    radius: 10
    clip: true
    color: Qt.alpha("#E2A5C4", 0.2)

    property alias indicator: indicatorId
    property real animationDuration: 200
    property bool animateIndicator: true

    function snapToY(y) {
        animateIndicator = false
        indicatorId.y = y
        animateIndicator = true
    }

    Rectangle {
        id: indicatorId
        width: parent.width
        height: 20
        radius: 10
        color: "#E2A5C4"
        y: -height

        Behavior on y {
            enabled: root.animateIndicator
            NumberAnimation {
                duration: root.animationDuration
                easing.type: Easing.OutCubic
            }
        }
    }
}