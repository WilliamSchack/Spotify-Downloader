import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

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
    Rectangle {
        id: popup
        anchors.fill: parent
        visible: true

        // Background Dim
        color: Qt.alpha("#000000", 0.4)

        // Popup
        Rectangle {
            anchors.centerIn: parent
            width: 600
            height: 231

            radius: 20

            gradient: Gradient {
                GradientStop { position: 0.0; color: Qt.alpha("#252126", 0.8) }
                GradientStop { position: 1.0; color: Qt.alpha("#3E2C40", 0.8) }
            }

            border.color: "#F8A8CE"
            border.width: 1

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 10

                Text {
                    Layout.fillWidth: true
                    horizontalAlignment: Qt.AlignHCenter

                    text: "Add Tracks"
                    color: "white"
                    font.bold: true
                    font.pixelSize: 30
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 5

                    // Download Link
                    RowLayout {
                        Layout.fillWidth: true
                        Layout.maximumHeight: 40
                        spacing: 5

                        TextField {
                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            placeholderText: qsTr("Enter a Song/Album/Playlist URL...")
                            font.pixelSize: 18

                            verticalAlignment: TextInput.AlignVCenter
                            leftPadding: 10

                            background: Rectangle {
                                radius: 3
                                color: "white"
                            }

                            //text: _manager.linkInputText
                            //onTextChanged: _manager.linkInputText = text
                        }

                        CustomButton {
                            Layout.preferredWidth: 40
                            Layout.preferredHeight: 40

                            radius: 3
                            hasIcon: false
                            color: "white"
                            //icon.source: "qrc:/icons/Icons/Clipboard_Icon_B.png"
                            // onClicked: _manager.PasteButtonClicked()
                        }
                    }

                    // Folder Path
                    RowLayout {
                        Layout.fillWidth: true
                        Layout.maximumHeight: 40
                        spacing: 5

                        TextField {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            placeholderText: qsTr("Select a Save Location ...")
                            font.pixelSize: 18

                            verticalAlignment: TextInput.AlignVCenter
                            leftPadding: 10

                            background: Rectangle {
                                radius: 3
                                color: "white"
                            }

                            //text: _manager.folderInputText
                            //onTextChanged: _manager.folderInputText = text
                        }

                        CustomButton {
                            Layout.preferredWidth: 40
                            Layout.preferredHeight: 40
                            width: height

                            radius: 3
                            hasIcon: false
                            color: "white"
                            //icon.source: "qrc:/icons/Icons/Folder_Icon_B.png"
                            //onClicked: _manager.FolderButtonClicked()
                        }
                    }
                }

                CustomButton {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    hasIcon: false
                    text: "Add Track(s) & Start Download"
                    font.bold: true
                    font.pixelSize: 16
                    color: "#9A607D"
                }
            }
        }
    }
}