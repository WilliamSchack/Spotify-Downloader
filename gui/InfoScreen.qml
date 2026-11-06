import QtQuick
import QtQuick.Layouts

RowLayout {
    id: root
    spacing: 10

    enum EInfoScreen
    {
        Updates,
        Notices,
        Supporters
    }

    property int currentInfoScreen: InfoScreen.EInfoScreen.Updates;

    function switchInfoScreen(screen: int) {
        currentInfoScreen = screen;
    }

    // Left Area
    Rectangle {
        Layout.fillHeight: true
        Layout.preferredWidth: 260

        radius: 10
        gradient: Gradient {
            GradientStop { position: 0.0; color: Qt.alpha("#373737", 0.8) }
            GradientStop { position: 1.0; color: Qt.alpha("#3D2B3F", 0.8) }
        }

        Item {
            anchors.fill: parent
            anchors.margins: 10

            ColumnLayout {
                anchors.fill: parent

                spacing: 15

                ToggleButtonGroup {
                    Layout.fillWidth: true

                    spacing: 5
                    horizontal: false

                    ToggleButton {
                        implicitWidth: parent.width
                        implicitHeight: 30

                        text: "Updates"
                        horizontalAlignment: Text.AlignLeft
                        font.pixelSize: 20
                        radius: 8

                        unselectedBackgroundColour: Qt.alpha("#707070", 0.6)

                        checked: true

                        onClicked: root.switchInfoScreen(InfoScreen.EInfoScreen.Updates)
                    }

                    ToggleButton {
                        implicitWidth: parent.width
                        implicitHeight: 30

                        text: "Notices"
                        horizontalAlignment: Text.AlignLeft
                        font.pixelSize: 20
                        radius: 8

                        unselectedBackgroundColour: Qt.alpha("#707070", 0.6)

                        onClicked: root.switchInfoScreen(InfoScreen.EInfoScreen.Notices)
                    }

                    ToggleButton {
                        implicitWidth: parent.width
                        implicitHeight: 30

                        text: "Supporters"
                        horizontalAlignment: Text.AlignLeft
                        font.pixelSize: 20
                        radius: 8

                        unselectedBackgroundColour: Qt.alpha("#707070", 0.6)

                        onClicked: root.switchInfoScreen(InfoScreen.EInfoScreen.Supporters)
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: "#E2A5C4"
                }

                StackLayout {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    
                    currentIndex: root.currentInfoScreen

                    InfoScreenUpdatesLeft {}
                    InfoScreenNoticesLeft {}
                    InfoScreenSupportersLeft {}
                }
            }
        }
    }

    // Right Area
    Rectangle {
        Layout.fillHeight: true
        Layout.fillWidth: true

        radius: 10
        gradient: Gradient {
            GradientStop { position: 0.0; color: Qt.alpha("#373737", 0.8) }
            GradientStop { position: 1.0; color: Qt.alpha("#3D2B3F", 0.8) }
        }

        Item {
            anchors.fill: parent
            anchors.margins: 20

            StackLayout {
                anchors.fill: parent

                currentIndex: root.currentInfoScreen

                InfoScreenUpdatesRight {}
                InfoScreenNoticesRight {}
                InfoScreenSupportersRight {}
            }
        }
    }
}