import QtQuick
import QtQuick.Controls.Basic

CustomButton {
    // Set by the CustomContextMenu
    property Menu menu: null

    implicitWidth: 100

    font.pixelSize: 15
    horizontalAlignment: Text.AlignLeft

    backgroundColour: "transparent"
    hoveredColour: Qt.alpha("white", 0.3)
    disabledColour: "transparent"

    onClicked: {
        if (menu !== null)
            menu.close()
    }
}