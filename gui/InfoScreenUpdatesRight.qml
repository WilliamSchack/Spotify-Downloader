import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts

ColumnLayout
{
    id: root

    width: parent.width
    height: parent.height

    spacing: 30

    RowLayout {
        Layout.fillWidth: true
        Layout.preferredHeight: 110
        Layout.maximumHeight: 110

        Image {
            Layout.preferredHeight: parent.height
            Layout.preferredWidth: parent.height
            
            source: "qrc:/icons/Icon.svg"
            sourceSize.width: parent.height
            sourceSize.height: parent.height
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Layout.topMargin: 10
            Layout.bottomMargin: 10

            spacing: 0

            Text {
                text: "Echolite"
                color: "white"
                font.pixelSize: 20
                font.bold: true
            }

            Text {
                text: "v2.0.0"
                color: "white"
                font.pixelSize: 16
                font.italic: true
            }

            Item { Layout.fillHeight: true }

            Row {
                Layout.fillWidth: true
                Layout.preferredHeight: 26

                spacing: 10

                CustomButton {
                    height: parent.height

                    text: "Check for Update"
                    font.pixelSize: 16
                    radius: 8

                    backgroundColour: "#A16583"
                }

                CustomButton {
                    height: parent.height

                    text: "Source"
                    font.pixelSize: 16
                    radius: 8

                    backgroundColour: "#A16583"
                }
            }
        }
    }

    Flickable {
        id: flickable

        Layout.fillWidth: true
        Layout.fillHeight: true

        contentWidth: width

        property bool scrollBarActive: contentHeight > height

        ScrollBar.vertical: ScrollBar {
            policy: flickable.scrollBarActive ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
        }
    }
}