import QtQuick

Column {
    id: root

    property int paragraphSpacing: 12
    property string text: ""
    property color colour: "white"
    property font font: Qt.font({
        pixelSize: 14
    })
    property var horizontalAlignment: Text.AlignLeft

    width: parent.width
    spacing: paragraphSpacing

    // Double newlines dont work properly in text, split them into seperate objects
    Repeater {
        model: root.text.split("\n\n")

        Text {
            required property string modelData

            width: root.width

            text: modelData
            color: root.colour
            font: root.font
            wrapMode: Text.Wrap
            textFormat: Text.MarkdownText
            horizontalAlignment: root.horizontalAlignment

            onLinkActivated: (link) => { Qt.openUrlExternally(link) }

            HoverHandler {
                cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
            }
        }
    }
}