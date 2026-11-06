import QtQuick
import QtQuick.Layouts

ToggleButton {
    id: root

    property alias iconSource: icon.source
    property alias name: nameText.text
    property alias version: versionText.text
    property bool upToDate: true

    implicitWidth: parent.width
    height: 50

    unselectedBackgroundColour: "transparent"
    selectedBackgroundColour: Qt.alpha("white", 0.3)

    radius: 6

    RowLayout {
        anchors.fill: parent

        spacing: 10

        Image {
            id: icon

            Layout.preferredHeight: parent.height - 15
            Layout.preferredWidth: parent.height - 15

            
            sourceSize.width: parent.height - 15
            sourceSize.height: parent.height - 15
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter

            spacing: 0

            Text {
                id: nameText

                color: "white"
                font.pixelSize: 16
                font.bold: true
            }

            RowLayout {
                Layout.fillWidth: true

                Text {
                    id: versionText

                    text: "v2.0.0"
                    color: "white"
                    font.pixelSize: 12
                    font.italic: true
                }

                Item { Layout.fillWidth: true }

                Text {
                    // TODO: Replace check/cross with icon
                    text: root.upToDate ? "✓ Up To Date" : "🡻 Update Available"
                    color: root.upToDate ? "#9EFFA5" : "#FF5B5D"
                    font.pixelSize: 12
                    font.italic: true
                }
            }
        }
    }
}