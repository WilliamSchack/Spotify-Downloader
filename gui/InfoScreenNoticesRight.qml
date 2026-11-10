import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts

ColumnLayout
{
    width: parent.width
    height: parent.height
    
    spacing: 20

    Header {
        Layout.fillWidth: true

        label: _manager.info.notices.selectedNotice.title
    }

    Flickable {
        id: flickable
        Layout.fillWidth: true
        Layout.fillHeight: true

        clip: true

        contentWidth: width
        contentHeight: contents.height
        
        property bool scrollBarActive: contentHeight > height

        ScrollBar.vertical: ScrollBar {
            policy: flickable.scrollBarActive ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
        }

        Text {
            id: contents

            width: parent.width - (flickable.scrollBarActive ? 20 : 0)
            height: 100

            text: _manager.info.notices.selectedNotice.content
            color: "white"
            font.pixelSize: 14
            wrapMode: Text.Wrap
            textFormat: Text.MarkdownText

            onLinkActivated: (link) => { Qt.openUrlExternally(link) }

            HoverHandler {
                cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
            }
        }
    }
}