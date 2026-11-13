import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Effects

ComboBox {
    id: root

    implicitHeight: parent.height
    implicitWidth: 300

    font.pixelSize: 14

    background: Rectangle {
        width: parent.width
        radius: 3

        InnerShadow {
            topEnabled: false
            leftEnabled: false

            radius: 3
            size: 4
            strength: 0.2
        }
    }

    contentItem: Text {
        height: root.height
        leftPadding: 10

        text: root.displayText
        color: "black"
        font: root.font
        
        verticalAlignment: Text.AlignVCenter
    }

    delegate: ItemDelegate {
        id: item

        required property string modelData
        required property int index

        width: root.width
        highlighted: root.highlightedIndex === index

        topPadding: 3
        bottomPadding: 3

        contentItem: Text {
            text: modelData
            color: "black"
            font.pixelSize: root.font.pixelSize
            font.family: root.font.family
            font.bold: root.currentIndex === item.index
        }

        background: Rectangle {
            color: item.highlighted ? Qt.alpha('#414141', 0.2) : "transparent"
        }
    }
    
    popup: Popup {
        y: root.height
        width: root.width
        implicitHeight: contentItem.implicitHeight + topPadding + bottomPadding
        
        topPadding: 4
        bottomPadding: 4
        leftPadding: 0
        rightPadding: 0

        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model: root.popup.visible ? root.delegateModel : null
            currentIndex: root.highlightedIndex
        }

        background: Rectangle {
            color: "white"
            bottomLeftRadius: 6
            bottomRightRadius: 6

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
    }

    indicator: Canvas {
        id: canvas
        x: root.width - width - root.rightPadding
        y: root.topPadding + (root.availableHeight - height) / 2
        width: 12
        height: 8
        contextType: "2d"

        onPaint: {
            // Drawing with inset to not clip
            var lw = 2
            var inset = lw / 2

            context.reset();
            context.strokeStyle = "black";
            context.lineWidth = lw
            context.lineCap = "round"
            context.lineJoin = "round"
            context.beginPath();
            context.moveTo(inset, inset);
            context.lineTo(width / 2, height - inset);
            context.lineTo(width - inset, inset);
            context.stroke();
        }
    }
}