import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import GongJianBo 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello Qt Android")

    FileTool {
        id: file_tool
    }

    Column {
        anchors.centerIn: parent
        spacing: 12
        Text {
            text: "Hello Qt Android"
        }
        Text {
            text: String("USB %1").arg(usbManager.isOpen ? usbManager.deviceInfo : "None")
        }
        Row {
            spacing: 20
            Button {
                id: btn_usb
                text: "USB"
                onClicked: {
                    usbManager.testUsb()
                }
            }
            Button {
                id: btn_uvc
                text: "UVC"
                onClicked: {
                    usbManager.testUvc()
                }
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
    }
}
