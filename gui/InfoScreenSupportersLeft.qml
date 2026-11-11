import QtQuick
import QtQuick.Layouts

Item {
    id: root

    width: parent.width
    height: parent.height

    CustomButton {
        width: parent.width
        height: 50

        backgroundColour: Qt.alpha("white", 0.3)
        radius: 6

        onClicked: Qt.openUrlExternally("https://ko-fi.com/williamschack")

        RowLayout {
            anchors.fill: parent

            spacing: 10

            Image {
                id: icon

                Layout.preferredHeight: parent.height - 15
                Layout.preferredWidth: parent.height - 15

                source: "qrc:/icons/Ko-fi.png"            
                sourceSize.width: parent.height - 15
                sourceSize.height: parent.height - 15
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter

                spacing: 0

                Text {
                    text: "Support Development"
                    color: "white"
                    font.pixelSize: 16
                    font.bold: true
                }

                Text {
                    text: "Thank you!"
                    color: "white"
                    font.pixelSize: 12
                    font.italic: true
                }
            }
        }
    }
}