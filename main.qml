import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello Qt Android")

    Column {
        anchors.centerIn: parent
        spacing: 12
        Text {
            text: "Hello Qt Android"
        }
        Text {
            text: "USB " + (usbManager.isOpen ? usbManager.deviceName : "None")
        }
    }
}
