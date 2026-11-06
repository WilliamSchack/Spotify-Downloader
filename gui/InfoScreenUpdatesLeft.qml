import QtQuick
import QtQuick.Layouts

Column
{
    id: root

    width: parent.width

    ToggleButtonGroup {
        width: root.width

        horizontal: false

        DependencySelectionButton {
            iconSource: "qrc:/icons/Icon.svg"
            name: "Echolite"
            version: "v2.0.0"
            upToDate: true

            checked: true
        }

        DependencySelectionButton {
            name: "ffmpeg"
            version: "2026-02-15"
            upToDate: false
        }

        DependencySelectionButton {
            name: "yt-dlp"
            version: "2026.03.03"
            upToDate: false
        }

        DependencySelectionButton {
            name: "QuickJs"
            version: "2026-06-04"
            upToDate: true
        }
    }
}