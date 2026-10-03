import QtQuick
import QtQuick.Controls

Switch {
    id: root
    
    implicitHeight: parent.height
    implicitWidth: 50

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
        }
    }
}