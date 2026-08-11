import QtQuick
import QtQuick.Layouts

Rectangle {
    id: root

    radius: 10
    color: "green"

    property alias text: label.text
    property alias font: label.font
    property alias hasIcon: icon.visible
    property alias spacing: layout.columnSpacing
    property real padding: 10
    property real iconPadding: 4
    property bool iconRight: false

    implicitWidth: layout.implicitWidth + 2 * padding
    implicitHeight: layout.implicitHeight + 2 * padding

    GridLayout {
        id: layout
        anchors.fill: parent
        anchors.leftMargin: root.padding;
        anchors.rightMargin: root.padding;
        columnSpacing: 10
        rows: 1

        // Icon
        Rectangle {
            id: icon
            Layout.alignment: Qt.AlignVCenter
            Layout.topMargin: root.iconPadding
            Layout.bottomMargin: root.iconPadding
            Layout.fillHeight: true
            Layout.preferredWidth: height
            Layout.column: root.iconRight ? 1 : 0

            color: "white"
        }

        // Text
        Text {
            id: label
            font.family: "Roboto"
            font.pixelSize: 20
            color: "white"
            text: ""

            Layout.alignment: Qt.AlignVCenter
            Layout.column: root.iconRight ? 0 : 1
        }
    }
}