import QtQuick
import QtQuick.Effects

Image {
    id: root

    property int radius: 10

    layer.enabled: true
    layer.effect: MultiEffect {
        maskEnabled: true
        maskSource: mask
        maskThresholdMin: 0.5
        maskSpreadAtMin: 1.0
    }

    Rectangle {
        id: mask
        anchors.fill: parent

        layer.enabled: true
        layer.smooth: true
        visible: false

        radius: root.radius
    }
}