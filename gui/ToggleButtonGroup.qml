import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item
{
    id: root

    property real spacing: 5
    property bool horizontal: true

    default property alias content: layout.data

    implicitWidth: layout.implicitWidth
    implicitHeight: layout.implicitHeight

    ButtonGroup {
        buttons: layout.children
    }

    GridLayout {
        id: layout
        anchors.fill: parent

        flow: root.horizontal ? GridLayout.LeftToRight : GridLayout.TopToBottom
        rows: root.horizontal ? 1 : -1
        columns: root.horizontal ? -1 : 1

        rowSpacing: root.spacing
        columnSpacing: root.spacing
    }
}