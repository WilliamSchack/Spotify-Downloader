import QtQuick

Item {
    property int verticalPadding: 8

    width: parent.width
    height: verticalPadding * 2 + 1

    Rectangle {
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width
        height: 1

        color: "#E2A5C4"
    }
}