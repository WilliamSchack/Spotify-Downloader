import QtQuick

Item {
    id: root

    required property Item target
    property Flickable flickable: null
    property Item activeLink: null
    property alias label: mainText.text
    property bool activeByDefault: false

    readonly property bool active: activeLink == root

    signal activated(Item link)

    width: parent.width
    height: 20

    Text {
        id: mainText
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        color: root.active ? "white" : Qt.alpha("white", 0.6)
        font.pixelSize: 16
    }

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: {
            root.activated(root)

            var targetPos = root.target.mapToItem(root.flickable, 0, 0)
            var targetY = root.flickable.contentY + targetPos.y
            var maxScroll = Math.max(0, root.flickable.contentHeight - root.flickable.height)
            root.flickable.contentY = Math.min(Math.max(0, targetY), maxScroll)
        }
    }
}