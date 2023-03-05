import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello Qt Android")

    Text {
        anchors.centerIn: parent
        text: "Hello Qt Android"
    }
}
