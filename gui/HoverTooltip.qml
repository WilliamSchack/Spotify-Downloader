import QtQuick
import QtQuick.Controls.Basic

Popup {
    id: root

    property string text: ""
    property color colour: "white"
    property int offsetX: 18
    property int offsetY: 18

    padding: 8
    margins: 4
    font.pixelSize: 12

    closePolicy: Popup.NoAutoClose
    modal: false
    focus: false

    background: StyledBackground {
        radius: 5
    }

    contentItem: Text {
        text: root.text
        color: root.colour
        font: root.font
        textFormat: Text.MarkdownText
        wrapMode: Text.WordWrap
    }

    onEnabledChanged: {
        if (enabled && hoverHandler.hovered)
            root.open()
        else if (!enabled)
            root.close()
    }

    function move(localPosition) {
        x = localPosition.x + offsetX
        y = localPosition.y + offsetY
    }

    HoverHandler {
        id: hoverHandler
        parent: root.parent
        
        onHoveredChanged: {
            if (hovered && root.enabled)
                root.open()
            else
                root.close()
        }

        onPointChanged: root.move(point.position)
    }
}