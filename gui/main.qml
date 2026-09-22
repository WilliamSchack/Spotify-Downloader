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

    readonly property string selectedButtonColour: "#A16583";
    readonly property string unselectedButtonColour: Qt.alpha("#3A3A3A", 0.5);

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
                // Should replace this and below with a new button group class
                headerButtonMusic.color = selectedButtonColour
                headerButtonInfo.color = unselectedButtonColour
                headerButtonSettings.color = unselectedButtonColour
                break;
            case Main.EScreen.Info:
                console.log("Switching to info screen")

                screenLoader.source = "InfoScreen.qml"
                headerButtonMusic.color = unselectedButtonColour
                headerButtonInfo.color = selectedButtonColour
                headerButtonSettings.color = unselectedButtonColour
                break;
            case Main.EScreen.Settings:
                console.log("Switching to settings screen")

                screenLoader.source = "SettingsScreen.qml"
                headerButtonMusic.color = unselectedButtonColour
                headerButtonInfo.color = unselectedButtonColour
                headerButtonSettings.color = selectedButtonColour
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
                CustomButton {
                    id: headerButtonMusic
                    text: "Music"
                    color: "#A16583"

                    Layout.fillHeight: true

                    onClicked: switchScreen(Main.EScreen.Main)
                }

                CustomButton {
                    id: headerButtonInfo
                    text: "Info"
                    color: Qt.alpha("#3A3A3A", 0.5)

                    Layout.fillHeight: true

                    onClicked: switchScreen(Main.EScreen.Info)
                }

                CustomButton {
                    id: headerButtonSettings
                    text: "Settings"
                    color: Qt.alpha("#3A3A3A", 0.5)

                    Layout.fillHeight: true

                    onClicked: switchScreen(Main.EScreen.Settings)
                }

                CustomButton {
                    id: headerButtonTest
                    text: "TESTING"
                    color: Qt.alpha("#3A3A3A", 0.5)

                    Layout.fillHeight: true

                    onClicked: popup.visible = true
                }

                // Space
                Rectangle { Layout.fillWidth: true }

                // Buttons (testing)
                Repeater {
                    model: 3
                    CustomButton {
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

    // Popups
    AddTracksPopup {
        id: addTracksPopups
    }
}