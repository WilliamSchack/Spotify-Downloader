import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import QtQuick.Effects

Dialog {
    id: root

    property string text: ""
    property int fadeDurationMs: 200

    parent: Overlay.overlay
    anchors.centerIn: parent

    width: 400
    padding: 10

    modal: true
    closePolicy: Popup.NoAutoClose

    background: StyledBackground {
        radius: 5

        // Outer Shadow
        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: "black"
            shadowOpacity: 0.25
            shadowBlur: 0.4
            shadowHorizontalOffset: 4
            shadowVerticalOffset: 4
        }
    }

    header: Text {
        text: root.title
        color: "white"
        font.pixelSize: 20
        font.bold: true
        horizontalAlignment: TextInput.AlignHCenter
        padding: root.padding
    }

    contentItem: Text {
        text: root.text
        color: "white"
        font.pixelSize: 14
        wrapMode: Text.WordWrap
        horizontalAlignment: TextInput.AlignHCenter
    }

    footer: DialogButtonBox {
        id: buttons
        standardButtons: root.standardButtons

        height: 50
        padding: root.padding
        spacing: 10
        background: null

        delegate: CustomButton {
            implicitHeight: parent.height
            backgroundColour: "#A16583"
        }
    }

    enter: Transition {
        NumberAnimation {
            property: "opacity"
            from: 0.0
            to: 1.0
            duration: root.fadeDurationMs
        }
    }

    exit: Transition {
        NumberAnimation {
            property: "opacity"
            from: 1.0
            to: 0.0
            duration: root.fadeDurationMs
        }
    }

    Overlay.modal: Rectangle {
        color: Qt.alpha("#000000", 0.4)

        Behavior on opacity {
            NumberAnimation {
                duration: root.fadeDurationMs
            }
        }
    }

    onClosed: {
        root.destroy()
    }
}