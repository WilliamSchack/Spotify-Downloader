import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

RowLayout {
    spacing: 10

    // Left Area
    Rectangle {
        Layout.fillHeight: true
        Layout.preferredWidth: 260

        radius: 10
        gradient: Gradient {
            GradientStop { position: 0.0; color: Qt.alpha("#373737", 0.8) }
            GradientStop { position: 1.0; color: Qt.alpha("#3D2B3F", 0.8) }
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

        // Will be changed to a list later
        Column {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 20

            // Header
            Column {
                width: parent.width
                height: 34
                spacing: 10

                Text {
                    text: "Output"
                    color: "white"
                    font.bold: true
                    font.pixelSize: 20
                }

                Rectangle {
                    width: parent.width
                    height: 1
                    color: "#E2A5C4"
                }
            }

            // Each subsection has its own column for lower spacing
            Column {
                width: parent.width
                spacing: 10

                // Subheader
                Text {
                    text: "Audio"
                    color: "white"
                    font.bold: true
                    font.pixelSize: 14
                }

                // Each setting should be able to have multiple of the below inputs stacked in a row

                // Dropdown
                RowLayout {
                    width: parent.width
                    height: 20

                    Text {
                        Layout.alignment: Qt.AlignVCenter
                        text: "Codec"
                        color: "white"
                        font.pixelSize: 14
                    }

                    // Filler
                    Rectangle { Layout.fillWidth: true }

                    // Dropdown
                    ComboBox {
                        Layout.fillHeight: true
                        Layout.preferredWidth: 300

                        font.pixelSize: 12

                        model: ["MP3", "M4A", "WAV"/*, ...*/]

                        background: Rectangle {
                            width: parent.width
                            radius: 3
                        }
                        
                        indicator: Canvas {
                            id: canvas
                            x: parent.width - width - parent.rightPadding
                            y: parent.topPadding + (parent.availableHeight - height) / 2
                            width: 12
                            height: 8
                            contextType: "2d"

                            onPaint: {
                                // Drawing with inset to not clip
                                var lw = 2
                                var inset = lw / 2

                                context.reset();
                                context.strokeStyle = "black";
                                context.lineWidth = lw
                                context.lineCap = "round"
                                context.lineJoin = "round"
                                context.beginPath();
                                context.moveTo(inset, inset);
                                context.lineTo(width / 2, height - inset);
                                context.lineTo(width - inset, inset);
                                context.stroke();
                            }

                        }
                    }
                }

                // Input
                RowLayout {
                    width: parent.width
                    height: 20

                    Text {
                        Layout.alignment: Qt.AlignVCenter
                        text: "Normalise Volume"
                        color: "white"
                        font.pixelSize: 14
                    }

                    // Filler
                    Rectangle { Layout.fillWidth: true }

                    // Input
                    TextField {
                        Layout.fillHeight: true
                        Layout.preferredWidth: 300

                        topPadding: 0
                        bottomPadding: 0
                        leftPadding: 8
                        rightPadding: rightLabel.width + rightLabel.anchors.rightMargin * 2

                        font.pixelSize: 12
                        color: "black"

                        validator: DoubleValidator {
                            bottom: -50.0
                            top: 50.0
                            decimals: 1
                            notation: DoubleValidator.StandardNotation
                        }

                        background: Rectangle {
                            width: parent.width
                            radius: 3
                        }

                        Text {
                            id: rightLabel
                            text: "dB"
                            color: parent.color
                            font: parent.font
                            anchors.right: parent.right
                            anchors.rightMargin: 8
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }
                }

                // Single Input
                RowLayout {
                    width: parent.width
                    height: 20
                    
                    Text {
                        Layout.alignment: Qt.AlignVCenter
                        text: "Single Input"
                        color: "white"
                        font.pixelSize: 14
                    }

                    // Filler
                    Rectangle { Layout.fillWidth: true }

                    // Single Input
                    TextField {
                        Layout.fillHeight: true
                        Layout.preferredWidth: parent.height

                        padding: 0
                        leftPadding: 0 // Needs to be set for text to be centered for some reason

                        horizontalAlignment: TextInput.AlignHCenter
                        verticalAlignment: TextInput.AlignVCenter

                        font.pixelSize: 12
                        color: "black"

                        maximumLength: 1

                        background: Rectangle {
                            width: parent.width
                            radius: 3
                        }
                    }
                }

                // Toggle Button
                RowLayout {
                    width: parent.width
                    height: 20
                    
                    Text {
                        Layout.alignment: Qt.AlignVCenter
                        text: "Codec"
                        color: "white"
                        font.pixelSize: 14
                    }

                    // Filler
                    Rectangle { Layout.fillWidth: true }

                    // Toggle Buttong
                    Switch {
                        // Replace these with implicit when seperating
                        Layout.fillHeight: true
                        Layout.preferredWidth: 40

                        contentItem: Item {}

                        indicator: Rectangle {
                            anchors.fill: parent
                            radius: 3
                            color: parent.checked ? "#9A607D" : "#9F9F9F"

                            // Knob thing
                            Rectangle {
                                width: parent.height - 4
                                height: parent.height - 4
                                y: 2
                                radius: 3
                                color: "white"

                                x: parent.parent.checked ? parent.width - width - 2 : 2
                            }
                        }
                    }
                }
            }
        }
    }
}