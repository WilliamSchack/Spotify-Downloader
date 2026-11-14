import QtQuick
import QtQuick.Layouts
import QtQuick.Shapes
import QtQuick.Effects

Item {
    id: root

    property alias label: text.text
    property string tooltip: ""
    property string error: ""

    default property alias content: inputs.children

    width: parent.width
    height: 25

    HoverHandler {
        id: hoverHandler

        onHoveredChanged: {
            if (hovered) {
                fillShape.show()
                return
            }

            fillShape.hide()
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 10

        // Tooltip
        Row {
            Layout.fillHeight: true
            anchors.verticalCenter: parent.verticalCenter

            spacing: 8

            // Text
            Text {
                id: text
                anchors.verticalCenter: parent.verticalCenter
                color: "white"
                font.pixelSize: 14
            }

            // Tooltip
            Image {
                width: parent.height - 10
                height: width
                anchors.verticalCenter: parent.verticalCenter
                
                visible: root.tooltip !== ""

                source: "qrc:/icons/ui/Help.svg"
                sourceSize.width: width
                sourceSize.height: height
            }

            HoverTooltip {
                enabled: root.tooltip !== ""
                text: root.tooltip
            }
        }

        // Indicator
        Item {
            id: fillParent
            Layout.fillHeight: true
            Layout.fillWidth: true

            onWidthChanged: {
                if (hoverHandler.hovered) {
                    fillShape.width = width
                    fillShape.x = 0
                }
            }

            Shape {
                id: fillShape
                height: parent.height
                width: 0

                property real animationTime: 300
                property bool animating: true

                ShapePath {
                    id: fillStroke
                    strokeWidth: 2
                    strokeColor: Qt.alpha("white", 0.4)
                    strokeStyle: ShapePath.DashLine
                    capStyle: ShapePath.RoundCap

                    dashPattern: [3, 2]

                    startX: 0
                    startY: fillShape.height / 2

                    PathLine {
                        x: fillShape.width
                        y: fillShape.height / 2
                    }

                    Behavior on strokeColor {
                        enabled: fillShape.animating
                        ColorAnimation {
                            duration: fillShape.animationTime
                            easing.type: Easing.OutCubic
                        }
                    }
                }

                function show() {
                    animating = false
                    x = parent.width / 2
                    width = 0
                    fillStroke.strokeColor = "transparent"
                    animating = true

                    x = 0
                    width = parent.width
                    fillStroke.strokeColor = Qt.alpha("white", 0.4)
                }

                function hide() {
                    x = parent.width / 2
                    width = 0
                    fillStroke.strokeColor = "transparent"
                }

                Behavior on x {
                    enabled: fillShape.animating
                    NumberAnimation {
                        duration: fillShape.animationTime
                        easing.type: Easing.OutCubic
                    }
                }
                
                Behavior on width {
                    enabled: fillShape.animating
                    NumberAnimation {
                        duration: fillShape.animationTime
                        easing.type: Easing.OutCubic
                    }
                }
            }
        }

        // TODO: Change to warning/error icon
        //       Add tooltip
        //       Dont allow changing screens and show popup if invalid
        Image {
            Layout.preferredHeight: parent.height - 5
            Layout.preferredWidth: parent.height - 5
            visible: root.error !== "" || opacity > 0
            opacity: root.error === "" ? 0 : 1

            source: "qrc:/icons/ui/Error.svg"
            sourceSize.width: width
            sourceSize.height: height

            layer.enabled: true
            layer.effect: MultiEffect {
                colorization: 1.0
                colorizationColor: "#FF7E80"
            }

            Behavior on opacity {
                NumberAnimation {
                    duration: 200
                    easing.type: Easing.OutCubic
                }
            }

            HoverTooltip {
                text: root.error
            }
        }

        Row {
            id: inputs
            Layout.alignment: Qt.AlignVCenter
            Layout.fillHeight: true
            spacing: 10

            HoverTooltip {
                enabled: root.error !== ""
                text: root.error
            }
        }
    }
}