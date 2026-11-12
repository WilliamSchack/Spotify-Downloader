import QtQuick
import QtQuick.Layouts
import QtQuick.Effects

Window {
    id: root
    visible: true
    width: 1400
    height: 800
    title: qsTr("Minimal Qml")

    flags: Qt.Window | Qt.FramelessWindowHint

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

        if (currentScreen != Main.EScreen.Settings && screen == Main.EScreen.Settings)
            _manager.settings.EnteringScreen()

        currentScreen = screen
    }

    // Background
    Item {
        anchors.fill: parent

        // Playlist image
        Image {
            id: backgroundImage
            anchors.fill: parent
            anchors.margins: -100

            visible: _manager.music.backgroundImagePath !== ""

            source: _manager.music.backgroundImagePath
            asynchronous: true
            fillMode: Image.PreserveAspectCrop

            opacity: 0.8

            layer.enabled: true
            layer.effect: MultiEffect {
                blurEnabled: true
                blur: 1.0
                blurMax: 40
                blurMultiplier: 2.0
            }
        }

        // Gradient overlay
        Rectangle {
            anchors.fill: parent
            
            gradient: Gradient {
                GradientStop { position: 0.0; color: Qt.alpha("#D4BBE9", 0.2) }
                GradientStop { position: 1.0; color: Qt.alpha("#A0D4D1", 0.4) }
            }
        }

        // Background / Overlay colour
        Rectangle {
            anchors.fill: parent

            color: _manager.music.backgroundImagePath === "" ? "#8f95d3" : Qt.alpha("#3B3B3B", 0.4)
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
                id: header

                Layout.maximumHeight: 30
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft
                Layout.bottomMargin: 4
                spacing: 15

                ToggleButtonGroup {
                    spacing: 15

                    ToggleButton {
                        implicitHeight: header.height

                        text: "Music"
                        font.pixelSize: 20
                        radius: 10

                        checked: true

                        onClicked: root.switchScreen(Main.EScreen.Main)
                    }

                    ToggleButton {
                        implicitHeight: header.height

                        text: "Info"
                        font.pixelSize: 20
                        radius: 10

                        onClicked: root.switchScreen(Main.EScreen.Info)
                    }

                    ToggleButton {
                        implicitHeight: header.height

                        text: "Settings"
                        font.pixelSize: 20
                        radius: 10

                        onClicked: root.switchScreen(Main.EScreen.Settings)
                    }
                }

                // Space / Draggable Area
                // TODO: Test this on other platforms
                MouseArea {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    onPressed: root.startSystemMove() 
                }

                // TODO: Change min/max/close button icons

                // Minimise
                CustomButton {
                    Layout.preferredWidth: parent.height
                    Layout.fillHeight: true

                    radius: 20
                    backgroundColour: "#434343"
                    text: "-"

                    onClicked: root.showMinimized()
                }

                // Maximise
                CustomButton {
                    Layout.preferredWidth: parent.height
                    Layout.fillHeight: true

                    radius: 20
                    backgroundColour: "#434343"
                    text: "o"

                    onClicked: root.visibility === Window.Maximized ? root.showNormal() : root.showMaximized()
                }

                // Quit
                CustomButton {
                    Layout.preferredWidth: parent.height
                    Layout.fillHeight: true

                    radius: 20
                    backgroundColour: "#434343"
                    text: "x"

                    onClicked: root.close()
                }
            }

            // Main screen
            StackLayout {
                Layout.fillHeight: true
                Layout.fillWidth: true

                currentIndex: root.currentScreen

                MusicScreen {}
                InfoScreen {}
                SettingsScreen {}

                // Outer Shadow
                layer.enabled: true
                layer.effect: MultiEffect {
                    shadowEnabled: true
                    shadowColor: "black"
                    shadowOpacity: 0.25
                    shadowBlur: 0.4
                    shadowHorizontalOffset: 2
                    shadowVerticalOffset: 2
                }
            }
        }
    }
}