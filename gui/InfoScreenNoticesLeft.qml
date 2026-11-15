import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts

Item {
    implicitWidth: parent.width
    implicitHeight: parent.height

    ButtonGroup {
        id: buttonsGroup
    }

    ListView {
        id: notices

        anchors.fill: parent

        reuseItems: true
        clip: true
        spacing: 5

        property bool scrollBarActive: contentHeight > height

        ScrollBar.vertical: ScrollBar {
            policy: notices.scrollBarActive ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
        }

        model: _manager.info.notices.notices

        delegate: NoticeSelectionButton {
            required property var modelData

            ButtonGroup.group: buttonsGroup
            width: ListView.view.width - (notices.scrollBarActive ? 20 : 0)

            info: modelData
        }
    }
}