import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GongJianBo.Declarative 1.0
import GongJianBo.Tools 1.0

// USB测试
Rectangle {
    id: control

    Column {
        anchors.centerIn: parent
        spacing: 20
        Text {
            text: String("USB %1").arg(USBManager.deviceInfo)
        }
        Button {
            id: btn_usb
            text: String("USB %1").arg((USBManager.deviceMode === USBManager.TestUsb && USBManager.isOpen) ? "[ON]" : "[OFF]")
            onClicked: {
                USBManager.testOpen(USBManager.TestUsb)
            }
        }
        Button {
            id: btn_uvc
            text: String("UVC %1").arg((USBManager.deviceMode === USBManager.TestUvc && USBManager.isOpen) ? "[ON]" : "[OFF]")
            onClicked: {
                USBManager.testOpen(USBManager.TestUvc)
            }
        }
        Rectangle {
            width: 300
            height: 200
            border.color: "black"
            VideoItem {
                id: video_item
                anchors.fill: parent
                anchors.margins: 1
                Connections {
                    target: USBManager
                    function onNewFrame(frame) {
                        video_item.updateFrame(frame)
                    }
                }
            }
        }
    }
}
