import QtQuick
import QtQuick.Layouts
import Echolite.Info.Updates

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

            onClicked: _manager.info.updates.SelectDependency(DependencyInfo.Echolite)
        }

        DependencySelectionButton {
            name: "ffmpeg"
            version: "2026-02-15"
            upToDate: false

            onClicked: _manager.info.updates.SelectDependency(DependencyInfo.Ffmpeg)
        }

        DependencySelectionButton {
            name: "yt-dlp"
            version: "2026.03.03"
            upToDate: false

            onClicked: _manager.info.updates.SelectDependency(DependencyInfo.Ytdlp)
        }

        DependencySelectionButton {
            name: "QuickJs"
            version: "2026-06-04"
            upToDate: true

            onClicked: _manager.info.updates.SelectDependency(DependencyInfo.Quickjs)
        }
    }
}