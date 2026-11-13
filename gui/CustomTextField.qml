import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Effects

// TODO: Visibly disable when enabled is false

TextField {
    id: root
    implicitHeight: parent.height
    implicitWidth: 300

    topPadding: 0
    bottomPadding: 0
    leftPadding: 8
    rightPadding: hasRightLabel ? rightLabel.width + rightLabel.anchors.rightMargin * 2 : 8

    font.pixelSize: 14
    color: enabled ? "black" : "grey"

    property bool hasRightLabel: false
    property bool valid: true
    property alias rightText: rightLabel.text

    background: Rectangle {
        width: root.width
        radius: 3

        color: root.valid ? "white" : '#ffdedd'

        Behavior on color {
            ColorAnimation {
                duration: 200
                easing.type: Easing.OutCubic
            }
        }

        InnerShadow {
            bottomEnabled: false
            rightEnabled: false

            radius: 4
            size: 6
            strength: 0.2
        }
    }

    Text {
        id: rightLabel
        visible: root.hasRightLabel
        color: root.color
        font: root.font
        anchors.right: root.right
        anchors.rightMargin: 8
        anchors.verticalCenter: root.verticalCenter
    }

    ContextMenu.menu: CustomContextMenu {
        CustomMenuItem {
            text: "Cut"
            enabled: root.enabled && root.selectedText.length > 0
            onClicked: root.cut()
        }

        CustomMenuItem {
            text: "Copy"
            enabled: root.selectedText.length > 0
            onClicked: root.copy()
        }

        CustomMenuItem {
            text: "Paste"
            enabled: root.canPaste
            onClicked: root.paste()
        }

        CustomMenuItem {
            text: "Delete"
            enabled: root.enabled && root.selectedText.length > 0
            onClicked: root.remove(root.selectionStart, root.selectionEnd)
        }

        CustomMenuItem {
            text: "Select All"
            enabled: root.length > 0
            onClicked: root.selectAll()
        }

        CustomMenuSeparator {}

        CustomMenuItem {
            text: "Undo"
            enabled: root.canUndo
            onClicked: root.undo()
        }

        CustomMenuItem {
            text: "Redo"
            enabled: root.canRedo
            onClicked: root.redo()
        }
    }
}