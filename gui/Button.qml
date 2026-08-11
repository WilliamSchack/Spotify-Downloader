import QtQuick
import QtQuick.Layouts

Rectangle {
    id: root

    radius: 10
    color: "green"

    property alias text: label.text
    property alias font: label.font
    property alias hasIcon: icon.visible
    property alias spacing: layout.spacing
    property real padding: 10

    implicitWidth: layout.implicitWidth + 2 * padding
    implicitHeight: layout.implicitHeight + 2 * padding

    RowLayout {
        id: layout
        anchors.fill: parent
        anchors.margins: root.padding
        spacing: 10

        // Icon
        Rectangle {
            id: icon
            anchors.verticalCenter: parent.verticalCenter
            Layout.fillHeight: true
            Layout.preferredWidth: height

            color: "white"
        }

        // Text
        Text {
            id: label
            anchors.verticalCenter: parent.verticalCenter
            font.family: "Roboto"
            font.pixelSize: 20
            color: "white"
            text: ""
        }
    }
}