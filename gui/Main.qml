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

    readonly property string selectedButtonColour: "#A16583";
    readonly property string unselectedButtonColour: Qt.alpha("#3A3A3A", 0.5);

    property int currentScreen: Main.EScreen.Main;

    // screen is type of EScreen
    function switchScreen(screen: int) {
        if (currentScreen == Main.EScreen.Settings && screen != Main.EScreen.Settings)  {
            // Cannot leave if any settings are invalid
            if (!_manager.settings.allSettingsValid)
                return;
            
            _manager.settings.LeavingScreen()
        }

        switch (screen) {
            case Main.EScreen.Main:
                screenLoader.source = "MusicScreen.qml"
                // Should replace this and below with a new button group class
                headerButtonMusic.color = selectedButtonColour
                headerButtonInfo.color = unselectedButtonColour
                headerButtonSettings.color = unselectedButtonColour
                break;
            case Main.EScreen.Info:
                screenLoader.source = "InfoScreen.qml"
                headerButtonMusic.color = unselectedButtonColour
                headerButtonInfo.color = selectedButtonColour
                headerButtonSettings.color = unselectedButtonColour
                break;
            case Main.EScreen.Settings:
                screenLoader.source = "SettingsScreen.qml"
                headerButtonMusic.color = unselectedButtonColour
                headerButtonInfo.color = unselectedButtonColour
                headerButtonSettings.color = selectedButtonColour
                break;
            default:
                return;
        }

        currentScreen = screen
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

                    onClicked: root.switchScreen(Main.EScreen.Main)
                }

                CustomButton {
                    id: headerButtonInfo
                    text: "Info"
                    color: Qt.alpha("#3A3A3A", 0.5)

                    Layout.fillHeight: true

                    onClicked: root.switchScreen(Main.EScreen.Info)
                }

                CustomButton {
                    id: headerButtonSettings
                    text: "Settings"
                    color: Qt.alpha("#3A3A3A", 0.5)

                    Layout.fillHeight: true

                    onClicked: root.switchScreen(Main.EScreen.Settings)
                }

                // Space
                Rectangle { Layout.fillWidth: true }

                // Minimise
                CustomButton {
                    Layout.preferredWidth: parent.height
                    Layout.fillHeight: true

                    radius: 20
                    color: "#434343"
                    hasIcon: false

                    onClicked: root.showMinimized()
                }

                // Maximise
                CustomButton {
                    Layout.preferredWidth: parent.height
                    Layout.fillHeight: true

                    radius: 20
                    color: "#434343"
                    hasIcon: false

                    onClicked: root.visibility === Window.Maximized ? root.showNormal() : root.showMaximized()
                }

                // Quit
                CustomButton {
                    Layout.preferredWidth: parent.height
                    Layout.fillHeight: true

                    radius: 20
                    color: "#434343"
                    hasIcon: false

                    onClicked: root.close()
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