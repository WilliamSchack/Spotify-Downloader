import QtQuick
import QtQuick.Layouts
import QtQuick.Shapes
import Echolite.Info.Updates

Item {
    id: root

    required property DependencyInfo info

    visible: !info.installed

    readonly property string updateAvailableText: {
        if (info.downloading)
            return "Downloading..."

        if (!info.installed)
            return "Not Installed!"
        
        return "Up To Date"
    }

    readonly property color updateAvailableColour: {
        if (info.downloading)
            return "#78d0ff"

        if (!info.installed)
            return "#ff7e80"
        
        return "#9EFFA5"
    }

    implicitWidth: parent.width
    implicitHeight: 50

    RowLayout {
        anchors.fill: parent

        spacing: 10

        Image {
            id: icon

            Layout.preferredHeight: parent.height - 15
            Layout.preferredWidth: Layout.preferredHeight

            source: root.info.iconSource            
            sourceSize.width: width
            sourceSize.height: height
        }

        Text {
            Layout.fillHeight: true

            text: root.info.name
            color: "white"
            font.pixelSize: 20
            font.bold: true
            verticalAlignment: Text.AlignVCenter
        }

        Item { Layout.fillWidth: true }

        Text {
            Layout.fillHeight: true

            text: root.updateAvailableText
            color: root.updateAvailableColour
            font.pixelSize: 20
            font.italic: true
            verticalAlignment: Text.AlignVCenter
        }
    }
}