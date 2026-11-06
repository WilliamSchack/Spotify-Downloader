import QtQuick
import QtQuick.Layouts
import Echolite.Info.Updates

ToggleButton {
    id: root

    required property DependencyInfo info

    implicitWidth: parent.width
    height: 50

    unselectedBackgroundColour: "transparent"
    selectedBackgroundColour: Qt.alpha("white", 0.3)

    radius: 6

    onClicked: _manager.info.updates.SelectDependency(info.type)

    RowLayout {
        anchors.fill: parent

        spacing: 10

        Image {
            id: icon

            Layout.preferredHeight: parent.height - 15
            Layout.preferredWidth: parent.height - 15

            source: root.info.iconSource            
            sourceSize.width: parent.height - 15
            sourceSize.height: parent.height - 15
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter

            spacing: 0

            Text {
                text: root.info.name
                color: "white"
                font.pixelSize: 16
                font.bold: true
            }

            RowLayout {
                Layout.fillWidth: true

                Text {
                    text: root.info.version
                    color: "white"
                    font.pixelSize: 12
                    font.italic: true
                }

                Item { Layout.fillWidth: true }

                Text {
                    // TODO: Replace check/cross with icon
                    text: root.info.upToDate ? "✓ Up To Date" : "🡻 Update Available"
                    color: root.info.upToDate ? "#9EFFA5" : "#FF5B5D"
                    font.pixelSize: 12
                    font.italic: true
                }
            }
        }
    }
}