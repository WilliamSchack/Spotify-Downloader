import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Echolite.Info.Updates

Column
{
    id: root

    width: parent.width
    height: parent.height

    Flickable {
        id: flickable
        anchors.fill: parent

        clip: true

        contentWidth: width
        contentHeight: contents.height
        
        property bool scrollBarActive: contentHeight > height

        ScrollBar.vertical: ScrollBar {
            policy: flickable.scrollBarActive ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
        }

        ToggleButtonGroup {
            id: contents

            width: parent.width - (flickable.scrollBarActive ? 20 : 0)

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
}