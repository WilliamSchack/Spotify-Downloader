import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts

ColumnLayout {
    width: parent.width
    height: parent.height
    
    spacing: 20

    Column {
        Layout.fillWidth: true

        spacing: 5

        Row {
            anchors.horizontalCenter: parent.horizontalCenter

            spacing: 10

            Text {
                text: "Thank you all for the support"
                color: "white"
                font.pixelSize: 20
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
            }

            Image {
                anchors.verticalCenter: parent.verticalCenter

                width: 20
                height: width

                source: "qrc:/icons/ui/Heart.svg"
                sourceSize.width: width
                sourceSize.height: width
                smooth: true
            }
        }

        MarkdownText {
            width: parent.width

            text: "If you want to support development, you can [donate on my Ko-fi](https://ko-fi.com/williamschack) :)"
            font.pixelSize: 16
            horizontalAlignment: Text.AlignHCenter
        }
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