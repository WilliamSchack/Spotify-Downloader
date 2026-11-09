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

    readonly property string updateAvailableText: {
        if (info.updating)
            return "🡻 Updating..."

        if (info.updateAvailable)
            return "🡻 Update Available"
        
        return "✓ Up To Date"
    }

    readonly property color updateAvailableColour: {
        if (info.updating)
            return '#5bc6ff'

        if (info.updateAvailable)
            return "#FF5B5D"
        
        return "#9EFFA5"
    }

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
                    text: root.updateAvailableText
                    color: root.updateAvailableColour
                    font.pixelSize: 12
                    font.italic: true
                }
            }
        }
    }
}