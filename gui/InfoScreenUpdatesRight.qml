import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Echolite.Info.Updates

ColumnLayout
{
    id: root

    readonly property DependencyInfo selectedDependency: _manager.info.updates.selectedDependency

    width: parent.width
    height: parent.height

    spacing: 30

    readonly property string updateButtonText: {
        if (root.selectedDependency.downloading)
            return "Downloading..."

        if (!root.selectedDependency.installed)
            return "Download"

        if (selectedDependency.updating)
            return "Downloading Update..."

        if (selectedDependency.checkingForUpdates)
            return "Checking for Updates..."

        if (selectedDependency.updateAvailable)
            return "Download Update"

        return "Check for Updates"
    }

    RowLayout {
        Layout.fillWidth: true
        Layout.preferredHeight: 100
        Layout.maximumHeight: 100

        spacing: 20

        Image {
            Layout.preferredHeight: parent.height
            Layout.preferredWidth: parent.height
            
            source: root.selectedDependency.iconSource
            sourceSize.width: parent.height
            sourceSize.height: parent.height
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Layout.topMargin: 5
            Layout.bottomMargin: 5

            spacing: 3

            Text {
                text: root.selectedDependency.name
                color: "white"
                font.pixelSize: 20
                font.bold: true
            }

            Text {
                text: root.selectedDependency.installed ? root.selectedDependency.version : "Not Installed!"
                color: root.selectedDependency.installed ? "white" : "#ff7e80"
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

                    text: root.updateButtonText
                    font.pixelSize: 16
                    radius: 8

                    backgroundColour: "#A16583"

                    enabled: !root.selectedDependency.checkingForUpdates && !root.selectedDependency.updating && !root.selectedDependency.downloading
                    visible: (root.selectedDependency.installed && !root.selectedDependency.installedOnSystem) || !root.selectedDependency.installed

                    onClicked: _manager.info.updates.CheckForUpdatesButtonClicked()
                }

                CustomButton {
                    height: parent.height

                    text: "Website"
                    font.pixelSize: 16
                    radius: 8

                    backgroundColour: "#A16583"

                    onClicked: _manager.info.updates.SourceButtonClicked()
                }
            }
        }
    }

    ListView {
        id: changelog

        Layout.fillWidth: true
        Layout.fillHeight: true

        reuseItems: true
        clip: true
        spacing: 30

        property bool scrollBarActive: contentHeight > height

        ScrollBar.vertical: ScrollBar {
            policy: changelog.scrollBarActive ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
        }

        model: root.selectedDependency.releases

        delegate: MajorSection {
            required property var modelData

            width: ListView.view.width - (changelog.scrollBarActive ? 20 : 0)
            label: modelData.name

            Text {
                width: parent.width

                text: modelData.body
                color: "white"
                font.pixelSize: 14
                wrapMode: Text.Wrap
                textFormat: TextEdit.MarkdownText

                onLinkActivated: (link) => { Qt.openUrlExternally(link) }

                HoverHandler {
                    cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                }
            }
        }

        Row {
            anchors.centerIn: parent
            visible: root.selectedDependency.loadingReleases
            spacing: 10
    
            BusyIndicator {
                running: root.selectedDependency.loadingReleases
                palette.dark: "white"
                height: 50
                width: 50
            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: "Loading Release Notes..."
                color: "white"
                font.bold: true
                font.pixelSize: 28
            }
        }

        Text {
            anchors.centerIn: parent
            visible: root.selectedDependency.failedToGetReleases
            text: "Failed to Get Release Notes"
            color: "white"
            font.bold: true
            font.pixelSize: 28
        }
    }
}