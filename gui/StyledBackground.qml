import QtQuick

// Popup
Rectangle {
    radius: 10

    anchors.margins: margins

    gradient: Gradient {
        GradientStop { position: 0.0; color: Qt.alpha("#252126", 0.8) }
        GradientStop { position: 1.0; color: Qt.alpha("#3E2C40", 0.8) }
    }

    border.color: "#F8A8CE"
    border.width: 1
}