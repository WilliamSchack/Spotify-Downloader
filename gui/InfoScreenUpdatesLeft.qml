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
            info: _manager.info.updates.GetDependency(DependencyInfo.Echolite)
            checked: true
        }

        DependencySelectionButton {
            info: _manager.info.updates.GetDependency(DependencyInfo.Ffmpeg)
        }

        DependencySelectionButton {
            info: _manager.info.updates.GetDependency(DependencyInfo.Ytdlp)
        }

        DependencySelectionButton {
            info: _manager.info.updates.GetDependency(DependencyInfo.Quickjs)
        }
    }
}