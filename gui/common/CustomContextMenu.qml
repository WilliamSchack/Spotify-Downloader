import QtQuick
import QtQuick.Controls.Basic

Menu {
    padding: 8
    font.pixelSize: 12

    // Change width to the largest item
    implicitWidth: {
        let maxWidth = 0
        for (let i = 0; i < count; i++) {
            const item = itemAt(i)
            if (item)
                maxWidth = Math.max(maxWidth, item.implicitWidth)
        }

        return maxWidth + leftPadding + rightPadding
    }

    background: StyledBackground {
        radius: 5
    }

    Component.onCompleted: {
        for (let i = 0; i < count; i++) {
            const item = itemAt(i)
            if (item.menu === null)
                item.menu = this
        }
    }
}