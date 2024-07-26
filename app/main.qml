import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import GongJianBo 1.0

Window {
    id: main_window
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello Qt Android")

    // 屏蔽 close 关闭，accepted = false 则不处理该操作
    onClosing: function(event) {
        event.accepted = false
        console.log("main_window close")
    }
    MouseArea {
        anchors.fill: parent
        onPressed: {
            console.log("main_window onPressed")
        }
        onReleased: {
            console.log("main_window onReleased")
        }
        onClicked: {
            console.log("main_window onClicked")
        }
    }

    FileTool {
        id: file_tool
    }

    Connections {
        target: USBManager
        function onNewFrame(frame) {
            video_item.updateFrame(frame)
        }
    }

    Column {
        anchors.centerIn: parent
        spacing: 12
        Text {
            text: "Hello Qt Android"
        }
        Text {
            text: String("USB %1").arg(USBManager.deviceInfo)
        }
        Row {
            spacing: 20
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
        }
        Rectangle {
            width: 300
            height: 200
            border.color: "black"
            VideoItem {
                id: video_item
                anchors.fill: parent
                anchors.margins: 1
            }
        }
        Row {
            spacing: 20
            Button {
                text: "Read"
                onClicked: {
                    file_label.text = file_tool.readTest()
                }
            }
            Button {
                text: "Write"
                onClicked: {
                    file_label.text = file_tool.writeTest()
                }
            }
            Text {
                id: file_label
                text: "-"
            }
        }
        Row {
            spacing: 20
            Button {
                text: "Pop"
                onClicked: {
                    my_pop.open()
                }
            }
        }
    }

    // 叠加 Dialog 时，内部 Dialog 需要 parent 可见才能显示
    Dialog {
        id: my_pop
        anchors.centerIn: Overlay.overlay
        width: 200
        height: 200
        modal: true
        padding: 0
        // closePolicy 会影响 ESC 响应以及安卓返回处理
        // 默认 Popup.CloseOnEscape | Popup.CloseOnPressOutside
        closePolicy: Dialog.CloseOnEscape
        Rectangle {
            anchors.fill: parent
            color: "red"
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    my_pop.close()
                }
            }
        }
    }
}
