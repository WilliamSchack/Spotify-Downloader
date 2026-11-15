import QtQuick

CustomButton {
    id: root

    property color unselectedBackgroundColour: Qt.alpha("#3A3A3A", 0.5)
    property color selectedBackgroundColour: "#A16583"

    checkable: true
    backgroundColour: checked ? selectedBackgroundColour : unselectedBackgroundColour
}