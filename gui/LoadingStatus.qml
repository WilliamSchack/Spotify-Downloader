import QtQuick

Item {
    id: root

    required property bool isLoading
    required property bool loadingFailed
    required property string contentLabel

    anchors.fill: parent

    Row {
        anchors.centerIn: parent
        visible: root.isLoading
        spacing: 10

        Image {
            height: 50
            width: 50

            source: "qrc:/icons/ui/Update.svg"
            sourceSize.width: width
            sourceSize.height: width

            RotationAnimation on rotation {
                from: 0
                to: 360
                duration: 1000
                loops: Animation.Infinite
                running: root.isLoading

                onRunningChanged: {
                    if (!running)
                        icon.rotation = 0
                }
            }
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: "Loading " + root.contentLabel + "..."
            color: "white"
            font.bold: true
            font.pixelSize: 28
        }
    }

    Text {
        anchors.centerIn: parent
        visible: root.loadingFailed
        text: "Failed to Get " + contentLabel
        color: "white"
        font.bold: true
        font.pixelSize: 28
    }
}