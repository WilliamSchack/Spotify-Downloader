import QtQuick
import QtQuick.Layouts
import QtQuick.Shapes

Item {
    id: root

    property alias label: text.text
    property bool valid: true
    default property alias content: inputs.children

    width: parent.width
    height: 25

    HoverHandler {
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

        Text {
            id: text
            Layout.alignment: Qt.AlignVCenter
            color: "white"
            font.pixelSize: 14
        }

        // Indicator
        Item {
            id: fillParent
            Layout.fillHeight: true
            Layout.fillWidth: true

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
        Rectangle {
            Layout.preferredHeight: parent.height - 5
            Layout.preferredWidth: parent.height - 5

            visible: !root.valid
            color: "red"
        }

        Row {
            id: inputs
            Layout.alignment: Qt.AlignVCenter
            Layout.fillHeight: true
            spacing: 10
        }
    }
}