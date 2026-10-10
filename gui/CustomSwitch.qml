import QtQuick
import QtQuick.Controls

Switch {
    id: root
    
    implicitHeight: parent.height
    implicitWidth: 50

    property bool initialized: false

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onPressed: (mouse) => { mouse.accepted = false }
    }

    contentItem: Item {}

    indicator: Rectangle {
        anchors.fill: parent
        radius: 3
        color: root.checked ? "#9A607D" : "#9F9F9F"

        // Knob thing
        Rectangle {
            width: parent.height - 4
            height: parent.height - 4
            y: 2
            radius: 3
            color: "white"

            x: root.checked ? parent.width - width - 2 : 2

            Behavior on x {
                enabled: root.initialized
                NumberAnimation {
                    duration: 200
                    easing.type: Easing.OutCubic
                }
            }
        }

        Behavior on color {
            ColorAnimation {
                duration: 200
                easing.type: Easing.OutCubic
            }
        }
    }

    Component.onCompleted: {
        initialized = true
    }
}