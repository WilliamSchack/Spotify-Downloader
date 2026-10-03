import QtQuick
import QtQuick.Controls

ComboBox {
    id: root

    implicitHeight: parent.height
    implicitWidth: 300

    font.pixelSize: 14

    background: Rectangle {
        width: parent.width
        radius: 3
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