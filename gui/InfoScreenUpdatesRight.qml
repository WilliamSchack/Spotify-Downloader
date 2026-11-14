import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Echolite.Info.Updates

ColumnLayout {
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

    readonly property string updateButtonIcon: {
        if (root.selectedDependency.downloading || !root.selectedDependency.installed)
            return "qrc:/icons/ui/Download.svg"

        if (selectedDependency.updateAvailable)
            return "qrc:/icons/ui/UpdateAvailable.svg"

        return "qrc:/icons/ui/Update.svg"
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
                    iconSource: root.updateButtonIcon
                    iconRotating: root.selectedDependency.checkingForUpdates
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
                    iconSource: "qrc:/icons/ui/External.svg"
                    iconMargins: 13
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

        // TODO: Fix the scrollbar changing height while scrolling
        ScrollBar.vertical: ScrollBar {
            policy: changelog.scrollBarActive ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
        }

        model: root.selectedDependency.releases

        delegate: MajorSection {
            required property var modelData

            width: ListView.view.width - (changelog.scrollBarActive ? 20 : 0)
            label: modelData.name

            MarkdownText {
                text: modelData.body
            }
        }

        LoadingStatus {
            isLoading: root.selectedDependency.loadingReleases
            loadingFailed: root.selectedDependency.failedToGetReleases
            contentLabel: "Release Notes"
        }
    }
}