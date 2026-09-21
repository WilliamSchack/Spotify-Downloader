// TEMPORARY, only used for repeaters
pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

Window
{
    id: root
    visible: true
    width: 1400
    height: 800
    title: qsTr("Minimal Qml")
    color: "#8f95d3"

    enum EScreen
    {
        Main,
        Info,
        Settings
    }

    // screen is type of EScreen
    function switchScreen(screen: int) {
        switch (screen) {
            case Main.EScreen.Main:
                console.log("Switching to music screen")
                screenLoader.source = "MusicScreen.qml"
                break;
            case Main.EScreen.Info:
                console.log("Switching to info screen")
                break;
            case Main.EScreen.Settings:
                console.log("Switching to settings screen")
                break;
            default:
                return;
        }
    }

    // Outer Margin
    Rectangle {
        anchors.fill: parent
        anchors.margins: 5
        color: "transparent"

        ColumnLayout {
            anchors.fill: parent
            spacing: 5

            // Header
            RowLayout {
                Layout.maximumHeight: 30
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft
                Layout.bottomMargin: 4
                spacing: 15

                // Buttons (testing)
                Button {
                    text: "Music"
                    color: "#A16583"

                    Layout.fillHeight: true

                    onClicked: switchScreen(Main.EScreen.Main)
                }

                Button {
                    text: "Info"
                    color: Qt.alpha("#3A3A3A", 0.5)

                    Layout.fillHeight: true

                    onClicked: switchScreen(Main.EScreen.Info)
                }

                Button {
                    text: "Settings"
                    color: Qt.alpha("#3A3A3A", 0.5)

                    Layout.fillHeight: true

                    onClicked: switchScreen(Main.EScreen.Settings)
                }

                // Space
                Rectangle { Layout.fillWidth: true }

                // Buttons (testing)
                Repeater {
                    model: 3
                    Button {
                        radius: 20
                        color: "#434343"
                        hasIcon: false

                        Layout.preferredWidth: parent.height
                        Layout.fillHeight: true
                    }
                }
            }

            // Main screen
            Loader {
                id: screenLoader

                Layout.fillHeight: true
                Layout.fillWidth: true

                source: "MusicScreen.qml"
            }
        }
    }
}