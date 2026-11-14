import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import QtQuick.Effects

Button {
    id: root

    property color textColour: "white"
    property url iconSource: ""
    property int iconMargins: 10
    property int iconSpacing: 5
    property color iconColour: "transparent"
    property bool iconRotating: false
    property color backgroundColour: "white"
    property color hoveredColour: Qt.darker(backgroundColour, 1.2)
    property color pressedColour: Qt.darker(backgroundColour, 1.5)
    property color disabledColour: Qt.darker(backgroundColour, 2.0)
    property real radius: 3

    property alias horizontalAlignment: label.horizontalAlignment
    property alias verticalAlignment: label.verticalAlignment

    default property alias content: contentParent.data
    readonly property bool hasContent: contentParent.children.length > 0

    leftPadding: 10
    rightPadding: 10
    topPadding: 0
    bottomPadding: 0

    implicitWidth: contentItem.implicitWidth + leftPadding + rightPadding
    implicitHeight: contentItem.implicitHeight + topPadding + bottomPadding

    font.family: "Roboto"
    font.pixelSize: 14

    // Text
    contentItem: Item {
        implicitWidth: root.hasContent ? contentParent.implicitWidth : noContentParent.implicitWidth
        implicitHeight: root.hasContent ? contentParent.implicitHeight : noContentParent.implicitHeight

        // Use text / icon if no content
        RowLayout {
            id: noContentParent
            anchors.fill: label.visible ? parent : null
            anchors.centerIn: label.visible ? null : parent
            visible: !root.hasContent
            spacing: root.iconSpacing

            Image {
                id: icon
                Layout.preferredWidth: root.height - root.iconMargins
                Layout.preferredHeight: Layout.preferredWidth
                Layout.alignment: Qt.AlignVCenter
                visible: root.iconSource !== "" && status === Image.Ready

                source: root.iconSource
                sourceSize.width: width
                sourceSize.height: width
                smooth: true

                opacity: root.enabled ? 1.0 : 0.5

                layer.enabled: root.iconColour !== "transparent"
                layer.effect: MultiEffect {
                    colorization: 1.0
                    colorizationColor: root.iconColour
                }

                RotationAnimation on rotation {
                    from: 0
                    to: 360
                    duration: 1000
                    loops: Animation.Infinite
                    running: root.iconRotating

                    onRunningChanged: {
                        if (!running)
                            icon.rotation = 0
                    }
                }
            }

            Text {
                id: label
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter
                visible: root.text !== ""

                text: root.text
                font: root.font
                opacity: enabled ? 1.0 : 0.3
                color: root.textColour
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        // Use custom content if assigned
        Item {
            id: contentParent
            visible: root.hasContent
            
            anchors.fill: parent
            opacity: enabled ? 1.0 : 0.3
        }
    }

    // Background
    background: Rectangle {
        color: root.enabled ? (root.down ? root.pressedColour : (root.hovered ? root.hoveredColour : root.backgroundColour)) : disabledColour
        radius: root.radius

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

        InnerShadow {
            visible: root.backgroundColour.a > 0

            topEnabled: false
            leftEnabled: false

            radius: root.radius
            size: 4
            strength: 0.2 * root.backgroundColour.a
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onPressed: (mouse) => { mouse.accepted = false }
    }
}