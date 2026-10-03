import QtQuick

Rectangle {
    id: root
    implicitWidth: 3
    radius: 10
    clip: true
    color: Qt.alpha("#E2A5C4", 0.2)

    property alias indicator: indicatorId
    property real animationDuration: 200

    Rectangle {
        id: indicatorId
        width: parent.width
        height: 20
        radius: 10
        color: "#E2A5C4"
        y: -height

        Behavior on y {
            NumberAnimation {
                duration: root.animationDuration
                easing.type: Easing.OutCubic
            }
        }
    }
}