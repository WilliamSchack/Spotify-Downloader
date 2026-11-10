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

        MarkdownText {
            id: contents
            width: parent.width - (flickable.scrollBarActive ? 20 : 0)

            text: _manager.info.notices.selectedNotice.content
        }
    }
}