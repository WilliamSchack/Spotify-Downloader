import QtQuick
import QtQuick.Layouts

Column {
    id: root

    required property string label
    required property real animationTime
    required property Flickable flickable
    property Item activeLink: null
    property alias track: trackId

    signal activated(Item link, Item group)

    default property alias content: links.children

    width: parent.width
    spacing: 10

    Text {
        width: parent.width
        text: root.label
        color: "white"
        font.bold: true
        font.pixelSize: 16
    }

    RowLayout {
        width: parent.width
        spacing: 10

        SettingsSidebarTrack {
            id: trackId
            Layout.fillHeight: true
            animationDuration: root.animationTime
        }

        Column {
            id: links
            Layout.fillWidth: true
            spacing: 8
        }
    }

    // After init setup each link
    Component.onCompleted: {
        for (let i = 0; i < links.children.length; i++) {
            let link = links.children[i]
            //link.track = trackId
            link.flickable = Qt.binding(() => root.flickable)
            link.activeLink = Qt.binding(() => root.activeLink)
            link.activated.connect((link) => root.activated(link, root))
        }
        
        // Activate default here so they are setup first
        // Not sure if its the best way of doing this but it works
        for (let i = 0; i < links.children.length; i++) {
            if (!links.children[i].activeByDefault)
                continue
            
            root.activated(links.children[i], root)
            break;
        }
    }
}