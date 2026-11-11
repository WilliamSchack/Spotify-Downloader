import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts

ColumnLayout
{
    width: parent.width
    height: parent.height
    
    spacing: 20

    Text {
        Layout.alignment: Qt.AlignHCenter

        text: "Thank you all for the support!"
        color: "white"
        font.bold: true
        font.pixelSize: 20
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

        Column {
            id: contents
            width: parent.width - (flickable.scrollBarActive ? 20 : 0)
            spacing: 20

            Header {
                label: "Diamond Supporters"
            }

            Header {
                label: "Gold Supporters"
            }

            Header {
                label: "Supporters"
            }

            Header {
                label: "One Time Supporters"
            }
        }
    }
}