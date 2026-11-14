import QtQuick

Item {
    id: root

    property url source: ""
    property int fadeDurationMs: 400
    property int fillMode: Image.PreserveAspectCrop

    property bool firstVisible: true

    Image {
        id: imageFirst

        anchors.fill: parent

        asynchronous: true
        fillMode: root.fillMode

        opacity: root.firstVisible ? 1 : 0

        Behavior on opacity {
            NumberAnimation {
                duration: root.fadeDurationMs
                easing.type: Easing.InOutCubic
            }
        }
    }

    Image {
        id: imageSecond

        anchors.fill: parent

        asynchronous: true
        fillMode: root.fillMode

        opacity: root.firstVisible ? 0 : 1

        Behavior on opacity {
            NumberAnimation {
                duration: root.fadeDurationMs
                easing.type: Easing.InOutCubic
            }
        }
    }

    onSourceChanged: {
        var nextImage = firstVisible ? imageSecond : imageFirst
        nextImage.source = source
    }

    Connections {
        // Show the image once its loaded
        target: root.firstVisible ? imageSecond : imageFirst
        function onStatusChanged() {
            var nextImage = firstVisible ? imageSecond : imageFirst
            if (nextImage.status === Image.Ready)
                firstVisible = !firstVisible
        }
    }
}