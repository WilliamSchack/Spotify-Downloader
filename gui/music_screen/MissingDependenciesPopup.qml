import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Effects
import Echolite.Info.Updates

Popup {
    id: root

    property int fadeDurationMs: 100

    anchors.centerIn: parent

    modal: false
    dim: false
    focus: true
    closePolicy: Popup.NoAutoClose

    padding: 20
    width: 560 + padding * 2

    background: StyledBackground {
        radius: 20

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

    contentItem: Column {
        spacing: 10

        Text {
            width: parent.width
            horizontalAlignment: Qt.AlignHCenter

            text: "Dependencies Not Installed"
            color: "white"
            font.bold: true
            font.pixelSize: 30
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter

            text: "Pleases install all the missing dependencies to use the app"
            color: "white"
            font.pixelSize: 18
        }

        Column {
            width: parent.width
            spacing: 0

            DependencyInstalledVisual {
                id: ffmpegVisual
                info: _manager.info.updates.GetDependency(DependencyInfo.Ffmpeg)
            }

            DependencyInstalledVisual {
                id: ytdlpVisual
                info: _manager.info.updates.GetDependency(DependencyInfo.Ytdlp)
            }

            DependencyInstalledVisual {
                id: qjsVisual
                info: _manager.info.updates.GetDependency(DependencyInfo.Quickjs)
            }
        }

        CustomButton {
            width: parent.width
            height: 40

            text: "Download Missing"
            font.bold: true
            font.pixelSize: 16
            backgroundColour: "#9A607D"

            onClicked: {
                if (ffmpegVisual.visible)
                    ffmpegVisual.info.Download()

                if (ytdlpVisual.visible)
                    ytdlpVisual.info.Download()

                if (qjsVisual.visible)
                    qjsVisual.info.Download()
            }
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
}