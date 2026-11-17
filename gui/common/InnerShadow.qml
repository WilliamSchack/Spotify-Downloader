import QtQuick
import QtQuick.Effects

Item {
    id: root

    property bool topEnabled: true
    property bool bottomEnabled: true
    property bool leftEnabled: true
    property bool rightEnabled: true

    property int size: 6
    property real strength: 0.2

    property real radius: 0
    
    anchors.fill: parent

    layer.enabled: true
    layer.effect: MultiEffect {
        maskEnabled: true
        maskSource: mask
    }

    // Top
    Rectangle {
        visible: root.topEnabled
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: root.size

        gradient: Gradient {
            GradientStop { position: 0.0; color: Qt.rgba(0, 0, 0, root.strength) }
            GradientStop { position: 0.4; color: Qt.rgba(0, 0, 0, root.strength * 0.4) }
            GradientStop { position: 1.0; color: Qt.rgba(0, 0, 0, 0) }
        }
    }

    // Bottom
    Rectangle {
        visible: root.bottomEnabled
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: root.size

        gradient: Gradient {
            GradientStop { position: 0.0; color: Qt.rgba(0, 0, 0, 0) }
            GradientStop { position: 0.4; color: Qt.rgba(0, 0, 0, root.strength * 0.4) }
            GradientStop { position: 1.0; color: Qt.rgba(0, 0, 0, root.strength) }
        }
    }

    // Left
    Rectangle {
        visible: root.leftEnabled
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        width: root.size

        gradient: Gradient {
            orientation: Gradient.Horizontal
            GradientStop { position: 0.0; color: Qt.rgba(0, 0, 0, root.strength) }
            GradientStop { position: 0.4; color: Qt.rgba(0, 0, 0, root.strength * 0.4) }
            GradientStop { position: 1.0; color: Qt.rgba(0, 0, 0, 0) }
        }
    }

    // Right
    Rectangle {
        visible: root.rightEnabled
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        width: root.size

        gradient: Gradient {
            orientation: Gradient.Horizontal
            GradientStop { position: 0.0; color: Qt.rgba(0, 0, 0, 0) }
            GradientStop { position: 0.4; color: Qt.rgba(0, 0, 0, root.strength * 0.4) }
            GradientStop { position: 1.0; color: Qt.rgba(0, 0, 0, root.strength) }
        }
    }

    // Mask
    Rectangle {
        id: mask
        anchors.fill: parent

        layer.enabled: true
        visible: false

        radius: root.radius
    }
}