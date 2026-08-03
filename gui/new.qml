import QtQuick
import QtQuick.Layouts

Window
{
    visible: true
    width: 1000
    height: 600
    title: qsTr("Minimal Qml")
    color: "purple"

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
                spacing: 15

                // Buttons (testing)
                Repeater {
                    model: 3
                    Rectangle {
                        Layout.preferredWidth: 120
                        Layout.fillHeight: true

                        radius: 10
                        color: "green"
                    }
                }

                // Space
                Rectangle { Layout.fillWidth: true }

                // Buttons (testing)
                Repeater {
                    model: 3
                    Rectangle {
                        Layout.preferredWidth: 30
                        Layout.fillHeight: true

                        radius: 20
                        color: "green"
                    }
                }
            }

            RowLayout {
                Layout.fillHeight: true
                Layout.fillWidth: true
                spacing: 10

                // Left Area
                Rectangle {
                    Layout.fillHeight: true
                    Layout.preferredWidth: 260

                    radius: 10
                    color: "blue"

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 10

                        // Group Header
                        ColumnLayout {
                            spacing: 10

                            RowLayout {
                                Layout.fillWidth: true

                                Text {
                                    text: "Groups"
                                    color: "white"
                                    font.family: "Roboto"
                                    font.bold: true
                                    font.pixelSize: 20
                                }

                                Rectangle { Layout.fillWidth: true }

                                Rectangle {
                                    Layout.preferredHeight: 25
                                    Layout.preferredWidth: 25

                                    radius: 5
                                    color: "green"
                                }
                            }

                            // Bottom border
                            Rectangle {
                                Layout.preferredHeight: 1
                                Layout.fillWidth: true
                                color: "green"
                            }
                        }

                        // Group Items
                        Repeater {
                            model: 2
                            Rectangle {
                                Layout.minimumHeight: 60
                                Layout.fillWidth: true

                                radius: 6
                                color: "grey"

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 4
                                    anchors.rightMargin: 8
                                    spacing: 10

                                    Rectangle {
                                        Layout.fillHeight: true
                                        Layout.minimumWidth: parent.height

                                        radius: 4
                                        color: "aqua"
                                    }

                                    ColumnLayout {
                                        Layout.fillHeight: true
                                        Layout.fillWidth: true
                                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                                        spacing: 4

                                        Text {
                                            Layout.fillWidth: true

                                            text: "Sample Group"
                                            color: "white"
                                            font.family: "Roboto"
                                            font.pixelSize: 16
                                        }

                                        Text {
                                            Layout.fillWidth: true

                                            text: "8 Tracks"
                                            color: "white"
                                            font.family: "Roboto"
                                            font.pixelSize: 12
                                        }
                                    }

                                    Text {
                                        Layout.fillHeight: true
                                        Layout.alignment: Qt.AlignRight

                                        text: ":"
                                        color: "white"
                                        font.family: "Roboto"
                                        font.pixelSize: 16
                                    }
                                }
                            }
                        }

                        // Space
                        Rectangle {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            color: "transparent"
                        }
                    }
                }

                // Right Area
                Rectangle {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    radius: 10
                    color: "red"

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                    }
                }
            }
        }
    }
}