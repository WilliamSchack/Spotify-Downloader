import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts

ColumnLayout {
    width: parent.width
    height: parent.height
    
    spacing: 20

    Text {
        Layout.alignment: Qt.AlignHCenter

        text: "Thank you all for the support!"
        color: "white"
        font.bold: true
        font.pixelSize: 20
    }

    ListView {
        id: changelog

        Layout.fillWidth: true
        Layout.fillHeight: true

        reuseItems: true
        clip: true
        spacing: 30

        property bool scrollBarActive: contentHeight > height

        ScrollBar.vertical: ScrollBar {
            policy: changelog.scrollBarActive ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
        }

        model: _manager.info.supporters.supporterSections

        delegate: SupporterSection {
            required property var modelData

            width: ListView.view.width - (changelog.scrollBarActive ? 20 : 0)
            
            info: modelData
        }

        LoadingStatus {
            isLoading: _manager.info.supporters.loadingSupporters
            loadingFailed: _manager.info.supporters.failedToGetSupporters
            contentLabel: "Supporters"
        }
    }
}