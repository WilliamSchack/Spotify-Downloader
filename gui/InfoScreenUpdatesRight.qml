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

    RowLayout {
        Layout.fillWidth: true
        Layout.preferredHeight: 110
        Layout.maximumHeight: 110

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

            Layout.topMargin: 10
            Layout.bottomMargin: 10

            spacing: 0

            Text {
                text: root.selectedDependency.name
                color: "white"
                font.pixelSize: 20
                font.bold: true
            }

            Text {
                text: root.selectedDependency.version
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

                    text: _manager.info.updates.updateButtonText
                    font.pixelSize: 16
                    radius: 8

                    backgroundColour: "#A16583"

                    enabled: !_manager.info.updates.checkingForUpdates

                    onClicked: _manager.info.updates.CheckForUpdatesButtonClicked()
                }

                CustomButton {
                    height: parent.height

                    text: "Source"
                    font.pixelSize: 16
                    radius: 8

                    backgroundColour: "#A16583"

                    onClicked: _manager.info.updates.SourceButtonClicked()
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