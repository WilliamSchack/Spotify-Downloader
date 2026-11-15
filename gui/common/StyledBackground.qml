import QtQuick

// Popup
Rectangle {
    radius: 10

    anchors.margins: margins

    gradient: Gradient {
        GradientStop { position: 0.0; color: "#2C252F" }
        GradientStop { position: 1.0; color: "#3B2B3E" }
    }

    border.color: "#F8A8CE"
    border.width: 1
}